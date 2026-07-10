#include "validation.h"

#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "stdint.h"

#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_mac.h"
#include "esp_system.h"
#include "esp_idf_version.h"
#include "esp_heap_caps.h"
#include "validation_gpio.h"
//#include "esp_psram.h"

void validation_init(void)
{
    printf("\n");
    printf("=====================================================\n");
    printf("           ESP32-S3 VALIDATION SUITE\n");
    printf("=====================================================\n");
    printf("\n");

    printf("[SYSTEM]\n");
    printf("Validation framework initialized.\n\n");
}


void validation_chip(void)
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    uint32_t flash_size = 0;
    esp_flash_get_size(NULL, &flash_size);

    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);

    printf("[ CHIP INFORMATION ]\n\n");

    printf("Chip          : ESP32-S3\n");
    printf("Revision      : v%d.%d\n",
           chip_info.revision / 100,
           chip_info.revision % 100);

    printf("CPU Cores     : %d\n", chip_info.cores);

    printf("IDF Version   : %s\n", esp_get_idf_version());

    printf("Flash Size    : %lu MB\n",
           flash_size / (1024 * 1024));

// #if CONFIG_SPIRAM
//     printf("PSRAM         : Enabled\n");
// #else
//     printf("PSRAM         : Disabled\n");
// #endif

    printf("Free Heap     : %u Bytes\n",
           (unsigned)esp_get_free_heap_size());

    printf("MAC Address   : %02X:%02X:%02X:%02X:%02X:%02X\n",
           mac[0], mac[1], mac[2],
           mac[3], mac[4], mac[5]);

    printf("\n");
}

void validation_memory(void)
{
    printf("[ MEMORY INFORMATION ]\n\n");

    /* ---------------- Internal RAM ---------------- */

    printf("Internal Heap\n");
    printf("----------------------------\n");

    printf("Free Heap           : %u Bytes\n",
           (unsigned)esp_get_free_heap_size());

    printf("Minimum Free Heap   : %u Bytes\n",
           (unsigned)esp_get_minimum_free_heap_size());

    printf("Largest Free Block  : %u Bytes\n",
           (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));

    printf("\n");

    /* ---------------- PSRAM ---------------- */

    printf("PSRAM\n");
    printf("---------------------------------\n");

    size_t psram_free =
        heap_caps_get_free_size(MALLOC_CAP_SPIRAM);

    size_t psram_largest =
        heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM);

    if (psram_free == 0)
    {
        printf("Status              : Not Available\n\n");
    }
    else
    {
        printf("Status              : Available\n");

        printf("Free Size           : %u Bytes\n",
               (unsigned)psram_free);

        printf("Largest Block       : %u Bytes\n",
               (unsigned)psram_largest);

        uint8_t *buffer =
            heap_caps_malloc(1024 * 1024,
                             MALLOC_CAP_SPIRAM);

        if (buffer)
        {
            memset(buffer, 0xAA, 1024 * 1024);

            printf("1 MB Allocation     : PASS\n");

            free(buffer);
        }
        else
        {
            printf("1 MB Allocation     : FAILED\n");
        }

        printf("\n");
    }
}