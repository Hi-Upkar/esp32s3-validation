#include "validation.h"

#include "esp_http_client.h"
#include "esp_log.h"

static const char *TAG = "HTTP";

static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;

        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;

        case HTTP_EVENT_HEADERS_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADERS_SENT");
            break;

        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG,
                     "Header: %s: %s",
                     evt->header_key,
                     evt->header_value);
            break;

        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG,
                     "Received %d bytes",
                     evt->data_len);
            break;

        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;

        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;

        default:
            break;
    }

    return ESP_OK;
}