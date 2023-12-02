#include "bmp280.h"
#include "logger.h"
#include "err_controller.h"
#include "errors_list.h"
#include "memory_utils.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

struct bmp_handle_obj {
    uint8_t id;
    bmp_config_t* config;
    spi_device_handle_t esp_handle;
};


#define BMP_CS_LOW_LEVEL        0
#define BMP_CS_HIGH_LEVEL       1

int bmp_init_chip_select(uint8_t gpio_cs) {
    err_c_t err = 0;

    gpio_config_t cs_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << gpio_cs,
        .pull_down_en = true,
    };
    err = gpio_config(&cs_config);
    
    return err;
}

int bmp_enable_cs(uint8_t cs) {
    return gpio_set_level(cs, BMP_CS_LOW_LEVEL);
}

int bmp_disable_cs(uint8_t cs) {
    return gpio_set_level(cs, BMP_CS_HIGH_LEVEL);
}

int bmp_send_cmd(bmp_handle_t bmp, const uint8_t cmd) {
    err_c_t err = 0;
    spi_transaction_t transaction;
    memutil_zero_memory(&transaction, sizeof(transaction));

    transaction.length = 8;
    transaction.tx_buffer = &cmd;
    transaction.user = (void*)0;
    transaction.flags = SPI_TRANS_CS_KEEP_ACTIVE;
    err = spi_device_polling_transmit(bmp->esp_handle, &transaction);

    return err;
}

int bmp_get_device_id(bmp_handle_t bmp) {
    err_c_t err = 0;
    spi_device_acquire_bus(bmp->esp_handle, portMAX_DELAY);

    bmp_send_cmd(bmp, 0x76);
    return err;
}

int bmp_add_device(bmp_config_t* in_config, bmp_handle_t out_handle) {
    err_c_t err = 0;

    bmp_init_chip_select(in_config->gpio_cs);
    bmp_disable_cs(in_config->gpio_cs);

    spi_bus_config_t bus_config = {
        .miso_io_num = in_config->gpio_miso,
        .mosi_io_num = in_config->gpio_mosi,
        .sclk_io_num = in_config->gpio_sclk,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
        .max_transfer_sz = 32,
    };
    err = spi_bus_initialize(in_config->host_id, &bus_config, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(err);

    spi_device_interface_config_t interface = {
        .clock_speed_hz = 10 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = in_config->gpio_cs,
        .queue_size = 8,
        .flags = SPI_DEVICE_POSITIVE_CS,
    };
    err = spi_bus_add_device(in_config->host_id, &interface, &(out_handle->esp_handle));
    ESP_ERROR_CHECK(err);
    return err;
}