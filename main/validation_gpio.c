#include "validation_gpio.h"

#include <stdio.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TEST_GPIO GPIO_NUM_2

void validation_gpio(void)
{
    printf("\n");
    printf("[ GPIO VALIDATION ]\n\n");

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << TEST_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&io_conf));

    printf("GPIO %d configured as OUTPUT\n", TEST_GPIO);
    printf("Beginning toggle test...\n\n");

    for (int i = 0; i < 10; i++)
    {
        gpio_set_level(TEST_GPIO, 1);
        printf("GPIO %d -> HIGH\n", TEST_GPIO);
        vTaskDelay(pdMS_TO_TICKS(500));

        gpio_set_level(TEST_GPIO, 0);
        printf("GPIO %d -> LOW\n", TEST_GPIO);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    printf("\nGPIO Toggle Test : PASS\n\n");
}