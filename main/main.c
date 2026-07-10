#include "validation.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    validation_init();
    validation_chip();
    validation_memory();
    validation_gpio();
    validation_wifi_station();
    validation_mqtt();

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}