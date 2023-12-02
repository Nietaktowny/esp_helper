#pragma once
#include <inttypes.h>

struct bmp_config_obj {
    uint8_t host_id;
    uint8_t gpio_sclk;
    uint8_t gpio_mosi;
    uint8_t gpio_miso;
    uint8_t gpio_cs;
};
typedef struct bmp_config_obj bmp_config_t;

typedef struct bmp_handle_obj* bmp_handle_t;

int bmp_init_chip_select(uint8_t gpio_cs);

int bmp_enable_cs(uint8_t cs);

int bmp_disable_cs(uint8_t cs);

int bmp_add_device(bmp_config_t* in_config, bmp_handle_t* out_handle);

int bmp_send_cmd(bmp_handle_t bmp, const uint8_t cmd);

int bmp_get_device_id(bmp_handle_t bmp);