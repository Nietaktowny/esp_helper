#include "bmp280.h"
#include "bmp280_internal.h"
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


#define BMP_CS_ENABLED        0
#define BMP_CS_DISABLED       1


/*! @name Interface settings */
#define BMP2_SPI_RD_MASK                              UINT8_C(0x80)
#define BMP2_SPI_WR_MASK                              UINT8_C(0x7F)
#define BMP2_REG_CHIP_ID                              UINT8_C(0xD0)

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
    return gpio_set_level(cs, BMP_CS_ENABLED);
}

int bmp_disable_cs(uint8_t cs) {
    return gpio_set_level(cs, BMP_CS_DISABLED);
}

int bmp_send_cmd(bmp_handle_t bmp, const uint8_t cmd) {
    err_c_t err = 0;
    spi_transaction_t transaction;
    memutil_zero_memory(&transaction, sizeof(transaction));

    transaction.length = 8;
    transaction.tx_buffer = &cmd;
    transaction.flags = SPI_TRANS_CS_KEEP_ACTIVE;
    err = spi_device_polling_transmit(bmp->esp_handle, &transaction);
    LOG_INFO("sended data: %u", cmd);
    return err;
}

int bmp_get_device_id(bmp_handle_t bmp) {
    err_c_t err = 0;
    uint8_t tx_data = 0xD0 | BMP2_SPI_RD_MASK;
    spi_transaction_t t;

    CHECK_NULL_PTR(bmp, LOG_ERROR("BMP280 handle cannot be NULL"));

    spi_device_acquire_bus(bmp->esp_handle, portMAX_DELAY);
    
    bmp_send_cmd(bmp, 0xE0 | BMP2_SPI_WR_MASK);
    vTaskDelay(pdMS_TO_TICKS(500));
    bmp_send_cmd(bmp, tx_data);
    
    memutil_zero_memory(&t, sizeof(t));
    t.length = 8;
    t.flags = SPI_TRANS_USE_RXDATA;
    err = spi_device_polling_transmit(bmp->esp_handle, &t);
    if(err != ESP_OK) {
        LOG_ERROR("error during SPI transmit: %s", esp_err_to_name(err));
        return err;
    }

    spi_device_release_bus(bmp->esp_handle);
    bmp_disable_cs(GPIO_NUM_21);
    return *(uint32_t*)t.rx_data;
}

int bmp_add_device(bmp_config_t* in_config, bmp_handle_t* out_handle) {
    err_c_t err = 0;

    CHECK_NULL_PTR(in_config, LOG_ERROR("BMP280 configuration cannot be NULL"));
    CHECK_NULL_PTR(out_handle, LOG_ERROR("bmp_handle_t cannot be NULL"));

    bmp_init_chip_select(in_config->gpio_cs);
    bmp_enable_cs(in_config->gpio_cs);

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
        .command_bits = 8,
        .address_bits = 8,
    };
    /*The idea is that we in most situations configure external devices at startup
    and use them for the whole time. Because of this here it's used lazy allocating - memory is allocated, and never freed.*/
    NEW((*out_handle), struct bmp_handle_obj);
    //(*out_handle) = malloc(sizeof(struct bmp_handle_obj));
    err = spi_bus_add_device(in_config->host_id, &interface, &((*out_handle)->esp_handle));
    ESP_ERROR_CHECK(err);

    (*out_handle)->id = in_config->host_id;

    LOG_INFO("registered new BMP280 device with ID: %u, ptr: %p", (*out_handle)->id, (*out_handle));
    return err;
}