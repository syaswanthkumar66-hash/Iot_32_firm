#include "udp_realtime.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "esp_log.h"
#include "session_manager.h"
#include "packet_validator.h"
#include "opcode_handler.h"
#include "app_config.h"
#include <string.h>

static const char *TAG = "UDP_SRV";
static int sock = -1;

void udp_realtime_init(void)
{
    // Socket will be created after IP is obtained
}

void udp_realtime_task(void)
{
    // Wait for WiFi connection first (simple delay)
    vTaskDelay(pdMS_TO_TICKS(5000));

    struct sockaddr_in server_addr;
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        ESP_LOGE(TAG, "socket create failed");
        vTaskDelete(NULL);
        return;
    }

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(UDP_CONTROL_PORT);

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        ESP_LOGE(TAG, "bind failed");
        close(sock);
        vTaskDelete(NULL);
        return;
    }
    ESP_LOGI(TAG, "UDP server listening on port %d", UDP_CONTROL_PORT);

    uint8_t rx_buffer[256] __attribute__((aligned(4)));
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while (1) {
        int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer), 0,
                           (struct sockaddr *)&client_addr, &addr_len);
        if (len < 0) {
            ESP_LOGE(TAG, "recvfrom error");
            continue;
        }
        if (len > 256) {
            ESP_LOGW(TAG, "Oversized packet dropped");
            continue;
        }

        // Packet processing inline (for latency) but with rate limit check
        if (packet_validator_check_rate_limit()) {
            ESP_LOGW(TAG, "Rate limit exceeded");
            continue;
        }

        // Extract session_id from packet (first 4 bytes after version)
        if (len < 5) continue;
        uint32_t session_id;
        memcpy(&session_id, rx_buffer + 1, 4);

        // Find session
        session_t *session = session_find(session_id);
        if (!session) {
            ESP_LOGW(TAG, "Unknown session ID 0x%08lx", session_id);
            continue;
        }

        // Decrypt and process
        uint8_t plain[256 - 1 - 4 - 4 - 12 - 16]; // approximate
        // ... full decryption and opcode handling
        // See packet_validator and opcode_handler
        int result = packet_process(rx_buffer, len, session, plain);
        if (result < 0) {
            ESP_LOGE(TAG, "Packet processing failed");
            continue;
        }
    }
}
