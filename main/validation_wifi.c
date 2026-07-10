#include "validation.h"

#include <stdlib.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

static const char *TAG = "WIFI";

/*Wi-Fi Configuration*/
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

/*Event Handler*/
#define WIFI_CONNECTED_BAT BIT0
#define WIFI_FAIL_BIT      BIT1

void validation_wifi(void)
{
    printf("\n");
    printf("[ WIFI STATION ]\n\n");

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

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    printf("Scanning...\n\n");

    ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));

    uint16_t number = 20;

    wifi_ap_record_t *ap_info =
        malloc(sizeof(wifi_ap_record_t) * number);

    if (ap_info == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }

    ESP_ERROR_CHECK(
        esp_wifi_scan_get_ap_records(&number, ap_info));

    printf("Found %d Access Points\n\n", number);

    for (int i = 0; i < number; i++)
    {
        printf("%2d | %-32s RSSI:%4d CH:%2d\n",
               i + 1,
               (char *)ap_info[i].ssid,
               ap_info[i].rssi,
               ap_info[i].primary);
    }

    free(ap_info);

    printf("\nWiFi Scan : PASS\n\n");

    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_deinit());
}