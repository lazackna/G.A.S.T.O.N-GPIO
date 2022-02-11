#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "mcp23017.h"

#define LYRAT_SCK 23
#define LYRAT_SDA 18

#define LCD_I2C_ADDR = 0x27
#define GPIO_I2C_ADDR = 0x20
#define AUDIO_I2C_ADDR = 0x10



void app_main(void)
{
    
}
