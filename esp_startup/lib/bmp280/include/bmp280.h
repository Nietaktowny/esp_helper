#pragma once
#include <inttypes.h>
#include <stddef.h>
#include "i2c_controller.h"

struct bmp_config_obj {
    uint8_t host_id;
    uint8_t gpio_sclk;
    uint8_t gpio_mosi;
    uint8_t gpio_miso;
    uint8_t gpio_cs;
};
typedef struct bmp_config_obj bmp_config_t;

typedef struct bmp_handle_obj* bmp_handle_t;

typedef enum bmp_mode_t {
    /** Sensor does no measurements. */
    BMP_MODE_SLEEP = 0,
    /** Sensor is in a forced measurement cycle. Sleeps after finishing. */
    BMP_MODE_FORCED = 1,
    /** Sensor does measurements. Never sleeps. */
    BMP_MODE_NORMAL = 3,
} bmp_mode_t;

typedef enum bmp_stdby_time_t {
    BMP_STANDBY_0M5 = 0x0,
    BMP_STANDBY_63M5 = 32,
    BMP_STANDBY_125M = 64,
    BMP_STANDBY_250M = 96,
    BMP_STANDBY_500M = 128,
    BMP_STANDBY_1000M = 160,
    BMP_STANDBY_2000M = 192,
    BMP_STANDBY_4000M = 224
} bmp_stdby_time_t;

typedef enum bmp_oversampling_t {
    BMP_OVERSAMPLING_SKIPPED = 0,
    BMP_OVERSAMPLING_X1 = 1,
    BMP_OVERSAMPLING_X2 = 2,
    BMP_OVERSAMPLING_X4 = 3,
    BMP_OVERSAMPLING_X8 = 4,
    BMP_OVERSAMPLING_X16 = 5
} bmp_oversampling_t;

typedef enum bmp_iir_filter_t {
    BMP_IIR_NONE = 0x0,
    BMP_IIR_X1,
    BMP_IIR_X2,
    BMP_IIR_X4,
    BMP_IIR_X8,
    BMP_IIR_X16,
} bmp_iir_filter_t;

// ERROR CODES
#define BMP_ERR_BASE                        0x0F00
#define BMP_ERR_NO_DEVICE_FOUND             BMP_ERR_BASE + 0x0001
#define BMP_ERR_WRONG_ADDR                  BMP_ERR_BASE + 0x0002
#define BMP_ERR_MODE_ERROR                  BMP_ERR_BASE + 0x0003

int bmp_init(bmp_handle_t* out_handle, i2c_c_bus_handle_t bus);

int bmp_i2c_read(bmp_handle_t bmp, uint8_t addr, uint8_t* out_buffer, size_t buflen);

int bmp_i2c_write(bmp_handle_t bmp, uint8_t addr, uint8_t* in_buffer, size_t buflen);

int bmp_i2c_identify_chip(bmp_handle_t bmp);

uint8_t bmp_get_chip_id(bmp_handle_t bmp);

uint8_t bmp_get_i2c_addr(bmp_handle_t bmp);

bmp_mode_t bmp_get_mode(bmp_handle_t bmp);

uint8_t bmp_is_measuring(uint8_t status);

uint8_t bmp_is_image_updating(uint8_t status);

int bmp_i2c_reset_chip(bmp_handle_t bmp);

int bmp_i2c_get_status(bmp_handle_t bmp, uint8_t* out_status);

int bmp_i2c_get_ctrl_meas(bmp_handle_t bmp, uint8_t* out_mode);

int bmp_i2c_get_config(bmp_handle_t bmp, uint8_t* out_config);

int bmp_i2c_set_mode(bmp_handle_t bmp, bmp_mode_t mode);

int bmp_i2c_set_standby_time(bmp_handle_t bmp, bmp_stdby_time_t time);

int bmp_i2c_set_press_oversampling(bmp_handle_t bmp, bmp_oversampling_t oversampling);

int bmp_i2c_set_temp_oversampling(bmp_handle_t bmp, bmp_oversampling_t oversampling);

int bmp_i2c_set_iir_filter(bmp_handle_t bmp, bmp_iir_filter_t filter);

int bmp_i2c_calibrate(bmp_handle_t bmp);

int32_t bmp_compensate_T_int32(bmp_handle_t bmp, int32_t adc_T);

uint32_t bmp_compensate_P_int64(bmp_handle_t bmp, int32_t adc_P);

int bmp_i2c_readout(bmp_handle_t bmp, int32_t* temperature, uint32_t* pressure);

/**
 * Convert sensor readout to floating point values.
 * @param tin Input temperature.
 * @param pin Input pressure.
 * @param hin Input humidity.
 * @param tout Output temperature. (C)
 * @param pout Output pressure. (Pa)
 * @param hout Output humidity. (%Rh)
 */
static inline void bmp_readout2float(int32_t* tin, uint32_t *pin, float *tout, float *pout)
{
    if (tin && tout)
        *tout = (float)*tin * 0.01f;
    if (pin && pout)
        *pout = (float)*pin * (1.0f/256.0f);
}

/**
 * Read sensor values as floating point numbers.
 * @param bmp Driver structure.
 * @param temperature The temperature in C.
 * @param pressure The pressure in Pa.
 * @param humidity The humidity in %RH.
 */
static inline int bmp_readoutFloat(bmp_handle_t bmp, float* temperature, float* pressure)
{
    int32_t t = 0; 
    uint32_t p = 0;
    int err = bmp_i2c_readout(bmp, &t, &p);

    if (err == 0)
    {
        bmp_readout2float(&t, &p, temperature, pressure);
    }

    return err;
};