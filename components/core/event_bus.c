#include "event_bus.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

#define EVENT_QUEUE_LEN 20
static QueueHandle_t event_queue;

esp_err_t event_bus_init(void)
{
    if (event_queue) return ESP_OK;
    event_queue = xQueueCreate(EVENT_QUEUE_LEN, sizeof(event_t));
    if (!event_queue) return ESP_ERR_NO_MEM;
    return ESP_OK;
}

esp_err_t event_bus_post(const event_t *evt)
{
    if (!event_queue) return ESP_FAIL;
    if (xQueueSend(event_queue, evt, 0) != pdTRUE)
        return ESP_ERR_TIMEOUT;
    return ESP_OK;
}

esp_err_t event_bus_receive(event_t *evt, TickType_t timeout)
{
    if (!event_queue) return ESP_FAIL;
    if (xQueueReceive(event_queue, evt, timeout) == pdTRUE)
        return ESP_OK;
    return ESP_ERR_TIMEOUT;
}
