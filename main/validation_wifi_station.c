#include "validation.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#define WIFI_SSID      "YOUR_WIFI_SSID"
#define WIFI_PASSWORD  "YOUR_WIFI_PASSWORD"
#define MAXIMUM_RETRY  5

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static EventGroupHandle_t s_wifi_event_group;
static const char *TAG = "WIFI";
static int s_retry_num = 0;

static void event_handler(void *arg,
                          esp_event_base_t event_base,
                          int32_t event_id,
                          void *event_data)
{
    if (event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(TAG, "Connecting to %s...", WIFI_SSID);
        ESP_ERROR_CHECK(esp_wifi_connect());
    }

    else if (event_base == WIFI_EVENT &&
         event_id == WIFI_EVENT_STA_DISCONNECTED)
{
    ESP_LOGW(TAG, "Wi-Fi disconnected. Reconnecting...");

    ESP_ERROR_CHECK(esp_wifi_connect());
}

    else if (event_base == IP_EVENT &&
             event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event =
            (ip_event_got_ip_t *)event_data;

        ESP_LOGI(TAG,
                 "IP Address: " IPSTR,
                 IP2STR(&event->ip_info.ip));

        s_retry_num = 0;

        xEventGroupSetBits(
            s_wifi_event_group,
            WIFI_CONNECTED_BIT);
    }
}

void validation_wifi_station(void)
{
    printf("\n");
    printf("[ WIFI STATION ]\n\n");

    s_wifi_event_group = xEventGroupCreate();

    if (s_wifi_event_group == NULL)
    {
        ESP_LOGE(TAG, "Failed to create Event Group");
        return;
    }

    esp_err_t ret;

    ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &event_handler,
            NULL,
            &instance_any_id));

    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &event_handler,
            NULL,
            &instance_got_ip));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(
        esp_wifi_set_mode(WIFI_MODE_STA));

    ESP_ERROR_CHECK(
        esp_wifi_set_config(
            WIFI_IF_STA,
            &wifi_config));

    ESP_ERROR_CHECK(
        esp_wifi_start());
    
    EventBits_t bits = xEventGroupWaitBits(
    s_wifi_event_group,
    WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
    pdFALSE,
    pdFALSE,
    portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT)
    {
    ESP_LOGI(TAG, "Wi-Fi Connected");
    }
    else if (bits & WIFI_FAIL_BIT)
    {
    ESP_LOGE(TAG, "Wi-Fi Connection Failed");
    }
    else
    {
    ESP_LOGE(TAG, "Unexpected Event");
    }
}
