idf_component_register(
    SRCS
        "main.c"
        "validation.c"
        "validation_gpio.c"
        "validation_wifi.c"
        "validation_wifi_station.c"
        "validation_mqtt.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES 
        spi_flash
        esp_psram
        esp_driver_gpio
        esp_wifi
        nvs_flash
        mqtt
)