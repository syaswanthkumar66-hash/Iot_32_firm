#include "udp_realtime.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "session_manager.h"
#include "packet_validator.h"
#include "opcode_handler.h"
#include "app_config.h"
#include "wifi_manager.h"
#include <string.h>

static const char *TAG = "UDP_SRV";
static int sock = -1;

void udp_realtime_init(void) { /* socket created in task */ }

void udp_realtime_task(void)
{
    // Wait for WiFi connection using event group
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT,
                                           pdFALSE, pdTRUE, pdMS_TO_TICKS(WIFI_CONNECT_TIMEOUT_MS));
    if (!(bits & WIFI_CONNECTED_BIT)) {
        ESP_LOGE(TAG, "WiFi not connected, aborting UDP server");
        vTaskDelete(NULL);
        return;
    }

    struct sockaddr_in server_addr;
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) { ESP_LOGE(TAG, "socket fail"); vTaskDelete(NULL); return; }

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(UDP_CONTROL_PORT);

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        ESP_LOGE(TAG, "bind fail");
        close(sock);
        vTaskDelete(NULL);
        return;
    }
    ESP_LOGI(TAG, "UDP server on port %d", UDP_CONTROL_PORT);

    uint8_t rx_buffer[MAX_ENCRYPTED_PACKET] __attribute__((aligned(4)));
    uint8_t plain[MAX_PLAIN_PAYLOAD];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    esp_task_wdt_add(NULL);

    while (1) {
        int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer), 0,
                           (struct sockaddr *)&client_addr, &addr_len);
        if (len < 0) continue;
        if (len > MAX_ENCRYPTED_PACKET) continue;

        if (packet_validator_check_rate_limit()) continue;

        if (len < 5) continue;
        uint32_t session_id;
        memcpy(&session_id, rx_buffer + 1, 4);

        session_t *session = session_find(session_id);
        if (!session) {
            ESP_LOGW(TAG, "Unknown session 0x%08lx", session_id);
            continue;
        }

        int ret = packet_process(rx_buffer, len, session, plain, sizeof(plain));
        if (ret < 0) {
            ESP_LOGE(TAG, "Packet process failed");
            continue;
        }
        esp_task_wdt_reset();
    }
}
