#include "validation_mqtt.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "inttypes.h"

static const char *TAG = "MQTT";

static void mqtt_event_handler(void *handler_args,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data)
{
    ESP_LOGI(TAG, "MQTT Event Received: %" PRId32, event_id);

    switch (event_id)
    {
        case MQTT_EVENT_CONNECTED:
        {
            ESP_LOGI(TAG, "MQTT Connected");

            int msg_id = esp_mqtt_client_subscribe(
                (esp_mqtt_client_handle_t)handler_args,
                "talksAI/test",
                0);

            ESP_LOGI(TAG, "Subscribe request sent, msg_id=%d", msg_id);
            break;
        }

        case MQTT_EVENT_SUBSCRIBED:
        {
            ESP_LOGI(TAG, "MQTT Subscribed");

            int msg_id = esp_mqtt_client_publish(
                (esp_mqtt_client_handle_t)handler_args,
                "talksAI/test",
                "{\"device\": \"ESP32-S3\", \"status\": \"online\"}",
                0,
                0,
                0);
            ESP_LOGI(TAG, "Publish request sent, msg_id=%d", msg_id);
            break;
        }

        case MQTT_EVENT_DISCONNECTED:
        {
            ESP_LOGW(TAG, "MQTT Disconnected");
            break;
        }

        case MQTT_EVENT_ERROR:
        {
            ESP_LOGE(TAG, "MQTT Error");
            break;
        }

        default:
        {
            ESP_LOGI(TAG, "Unhandled MQTT Event");
            break;
        }

        case MQTT_EVENT_PUBLISHED:
        {
            ESP_LOGI(TAG, "MQTT Published");
            break;
        }

        case MQTT_EVENT_DATA:
        {
            esp_mqtt_event_handle_t event = event_data;
            
            ESP_LOGI(TAG, "Topic: %.*s", 
                event->topic_len, 
                event->topic);

            ESP_LOGI(TAG, "Data: %.*s",
                event->data_len,
                event->data);

            break;
        }
    }
}

void validation_mqtt(void)
{
    esp_mqtt_client_handle_t mqtt_client = NULL;

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://test.mosquitto.org",
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    ESP_LOGI(TAG, "MQTT Client Handle = %p", mqtt_client);

    if (mqtt_client == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize MQTT Client");
        return;
    }

    esp_mqtt_client_register_event(
        mqtt_client,
        ESP_EVENT_ANY_ID,
        mqtt_event_handler,
        mqtt_client);

    esp_err_t ret = esp_mqtt_client_start(mqtt_client);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start MQTT client");
        return;
    }

    ESP_LOGI(TAG, "MQTT client started successfully");
}