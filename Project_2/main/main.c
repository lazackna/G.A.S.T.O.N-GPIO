#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "mcp23017.h"
#include "i2c_bus.h"

#define LYRAT_SCK 23
#define LYRAT_SDA 18

#define LYRAT_SCK_SPEED 10000

#define LCD_I2C_ADDR 0x27
#define GPIO_I2C_ADDR 0x20
#define AUDIO_I2C_ADDR 0x10

mcp23017_handle_t mcp23017;



TaskHandle_t Task1;
TickType_t lastWakeTime1;
TaskHandle_t Task2;
TickType_t lastWakeTime2;



SemaphoreHandle_t buttonMutex;
uint8_t buttonField = 0x00;

void Rood (void * parameter) 
{
    lastWakeTime1 = xTaskGetTickCount();
    static int isON = 0;
    while (1)
    {

        xSemaphoreTake(buttonMutex, portMAX_DELAY);
        uint8_t gpioA = mcp23017_read_io(mcp23017, MCP23017_GPIOA);

        if(gpioA & 0x01 && isON) 
        {
            mcp23017_write_io(mcp23017, gpioA | 0x01, MCP23017_GPIOB);
            isON = 0;     
        }
        else 
        {
            mcp23017_write_io(mcp23017, gpioA & ~0x01, MCP23017_GPIOB);
            isON = 1;
        }
        xSemaphoreGive(buttonMutex);
        vTaskDelay(350 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void Geel (void * parameter) 
{
    lastWakeTime2 = xTaskGetTickCount();

    static int isON = 0;
    while (1)
    {

        xSemaphoreTake(buttonMutex, portMAX_DELAY);
        uint8_t gpioA = mcp23017_read_io(mcp23017, MCP23017_GPIOA);

        if(gpioA & 0x02 && isON) 
        {
            mcp23017_write_io(mcp23017, gpioA | 0x02, MCP23017_GPIOB);
            isON = 0;     
        }
        else 
        {
            mcp23017_write_io(mcp23017, gpioA & ~0x02, MCP23017_GPIOB);
            isON = 1;
        }
        xSemaphoreGive(buttonMutex);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .scl_io_num = LYRAT_SCK,
        .sda_io_num = LYRAT_SDA,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = LYRAT_SCK_SPEED
    };

    i2c_bus_handle_t i2c_bus = i2c_bus_create(I2C_NUM_0, &i2c_conf);
    mcp23017 = mcp23017_create(i2c_bus, GPIO_I2C_ADDR);

    mcp23017_set_io_dir(mcp23017, 0x01, MCP23017_GPIOA);
    mcp23017_set_io_dir(mcp23017, 0x00, MCP23017_GPIOB);

    buttonMutex = xSemaphoreCreateMutex();
    
    xTaskCreatePinnedToCore(Rood, "Task1", 10000, NULL, 6, &Task1, 1);
    xTaskCreatePinnedToCore(Geel, "Task2", 10000, NULL, 6, &Task2, 1);
}
