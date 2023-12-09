#pragma once
#include <inttypes.h>
#include <stddef.h>
#include "i2c_controller.h"

/**
 * @brief BMP280 device handle.
*/
typedef struct bmp_handle_obj* bmp_handle_t;

/**
 * @brief BMP operation modes.
 * 
 * - BMP_MODE_SLEEP - Sensor does no measurements.
 * - BMP_MODE_FORCED - Sensor is in a forced measurement cycle. Sleeps after finishing.
 * - BMP_MODE_NORMAL - Sensor does measurements. Never sleeps.
 * 
 * @enum BMP operation modes.
*/
typedef enum bmp_mode_t {
    BMP_MODE_SLEEP = 0,                         ///< Sensor does no measurements.
    BMP_MODE_FORCED = 1,                        ///< Sensor is in a forced measurement cycle. Sleeps after finishing.
    BMP_MODE_NORMAL = 3,                        ///< Sensor does measurements. Never sleeps. 
} bmp_mode_t;

/**
 * @brief BMP standby time in normal operating mode.
 * 
 * @enum bmp_stdby_time_t
*/
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

/**
 * @brief BMP oversampling possible values.
 * 
 * @enum bmp_oversampling_t
*/
typedef enum bmp_oversampling_t {
    BMP_OVERSAMPLING_SKIPPED = 0,
    BMP_OVERSAMPLING_X1 = 1,
    BMP_OVERSAMPLING_X2 = 2,
    BMP_OVERSAMPLING_X4 = 3,
    BMP_OVERSAMPLING_X8 = 4,
    BMP_OVERSAMPLING_X16 = 5
} bmp_oversampling_t;

/**
 * @brief BMP IIR filter possible values.
 * 
 * @enum bmp_iir_filter_t
*/
typedef enum bmp_iir_filter_t {
    BMP_IIR_NONE = 0x0,
    BMP_IIR_X1,
    BMP_IIR_X2,
    BMP_IIR_X4,
    BMP_IIR_X8,
    BMP_IIR_X16,
} bmp_iir_filter_t;

/**
 * @brief BMP device config object.
 * 
 * @struct BMP device config object.
*/
struct bmp_config_obj {
    bmp_mode_t mode;                            ///< Operating mode of BMP280 device.
    bmp_stdby_time_t standby;                   ///< Standby of BMP280 device in normal operating mode.
    bmp_oversampling_t press_over;              ///< Oversampling of pressure.
    bmp_oversampling_t temp_over;               ///< Oversampling of temperature.
    bmp_iir_filter_t iir_filter;                ///< Value for IIR filter.
};
/**
 * @brief BMP device configuration.
*/
typedef struct bmp_config_obj bmp_config_t;

// ERROR CODES
#define BMP_ERR_BASE                        0x0F00                          ///< BMP base for errors.
#define BMP_ERR_NO_DEVICE_FOUND             BMP_ERR_BASE + 0x0001           ///< No BMP280 device was found on bus.
#define BMP_ERR_MODE_ERROR                  BMP_ERR_BASE + 0x0003           ///< Cannot change BMP device operating mode.

#define BMP_STANDARD_SEA_LEVEL_PRESSURE     1013.25                         ///< Standard pressure on sea level in hPa.

/**
 * @brief Initialize BMP device, and return it's handle.
 * 
 * @warning This function will exit if memory allocation won't work.
 * 
 * @param out_handle Buffer to store BMP280 device handle.
 * @param bus I2C bus handle, to which BMP device is connected.
 * 
 * @retval - 0 on success.
 * @retval - BMP_ERR_NO_DEVICE_FOUND - if no BMP device was found on bus.
 * @retval - BMP_ERR_MODE_ERROR - if cannot change device mode.
 * @retval - i2c_controller specific error codes.
 * @retval - esp specific error codes.
*/
int bmp_init(bmp_handle_t* out_handle, i2c_c_bus_handle_t bus);

/**
 * @brief Read register value of BMP280 device.
 * 
 * @param bmp BMP device handle.
 * @param addr Address of register to read.
 * @param out_buffer[out] Buffer to store result.
 * @param buflen Size of buffer.
 * 
 * @retval - 0 on success.
 * @retval - i2c_controller specific error codes.
 * @retval - esp specific error codes.
*/
int bmp_i2c_read(bmp_handle_t bmp, uint8_t addr, uint8_t* out_buffer, size_t buflen);

/**
 * @brief Write to register of BMP280 device.
 * 
 * @param bmp BMP device handle.
 * @param addr Address of register to write to.
 * @param out_buffer Buffer of the values to write.
 * @param buflen Size of buffer.
 * 
 * @retval - 0 on success.
 * @retval - i2c_controller specific error codes.
 * @retval - esp specific error codes.
*/
int bmp_i2c_write(bmp_handle_t bmp, uint8_t addr, uint8_t* in_buffer, size_t buflen);

/**
 * @brief Search for BMP280 device on possible addresses.
 * 
 * It also fetches chip ID.
 * 
 * @param bmp BMP device handle.
 * 
 * @retval - 0 on success
 * @retval - ERR_NULL_POINTER - when BMP device handle is NULL.
 * @retval - esp specific error codes
*/
int bmp_i2c_identify_chip(bmp_handle_t bmp);

/**
 * @brief Get BMP device chip ID.
 * 
 * @param bmp BMP device handle.
 * 
 * @return BMP device chip ID.
*/
uint8_t bmp_get_chip_id(bmp_handle_t bmp);

/**
 * @brief Get BMP device I2C address.
 * 
 * @param bmp BMP device handle.
 * 
 * @return BMP device I2C address.
*/
uint8_t bmp_get_i2c_addr(bmp_handle_t bmp);

/**
 * @brief Get current mode of BMP device.
 * 
 * @param bmp BMP device handle.
 * 
 * @retval - BMP_MODE_SLEEP 
 * @retval - BMP_MODE_FORCED
 * @retval - BMP_MODE_NORMAL
*/
bmp_mode_t bmp_get_mode(bmp_handle_t bmp);

/**
 * @brief Check if BMP device is currently measuring.
 * 
 * @param status Current value of status register.
 * 
 * @retval - 1 if it's measuring
 * @retval - 0 if it's not measuring
*/
uint8_t bmp_is_measuring(uint8_t status);

/**
 * @brief Check if BMP device is image updating.
 * 
 * @param status value of status register
 * 
 * @retval - 1 if it's image updating
 * @retval - 0 if it's not image updating
*/
uint8_t bmp_is_image_updating(uint8_t status);

/**
 * @brief Reset BMP chip.
 * 
 * @param bmp BMP device handle.
 * 
 * @retval - 0 on success
 * @retval - ERR_NULL_POINTER - when BMP device handle is NULL.
 * @retval - esp specific error codes
*/
int bmp_i2c_reset_chip(bmp_handle_t bmp);

/**
 * @brief Get value of BMP_REG_STATUS register using I2C.
 * 
 * @param bmp BMP device handle.
 * @param out_status[out] Location where store register value.
 * 
 * @retval - 0 on success
 * @retval - ERR_NULL_POINTER - when BMP device handle is NULL.
 * @retval - esp specific error codes
*/
int bmp_i2c_get_status(bmp_handle_t bmp, uint8_t* out_status);

/**
 * @brief Get value of BMP_REG_CTRL_MEAS register using I2C.
 * 
 * @param bmp BMP device handle.
 * @param out_ctrl[out] Location where store register value.
 * 
 * @retval - 0 on success
 * @retval - ERR_NULL_POINTER - when BMP device handle is NULL.
 * @retval - esp specific error codes
*/
int bmp_i2c_get_ctrl_meas(bmp_handle_t bmp, uint8_t* out_mode);

/**
 * @brief Get value of BMP_REG_CONFIG register using I2C.
 * 
 * @param bmp BMP device handle.
 * @param out_config[out] Location where store register value.
 * 
 * @retval - 0 on success
 * @retval - ERR_NULL_POINTER - when BMP device handle is NULL.
 * @retval - esp specific error codes
*/
int bmp_i2c_get_config(bmp_handle_t bmp, uint8_t* out_config);

/**
 * @brief Change BMP device operating mode.
 * 
 * @param bmp BMP device handle.
 * @param mode One of three operating modes of BMP280.
 * 
 * @retval - 0 on success
 * @retval - ERR_NULL_POINTER - when BMP device handle is NULL.
 * @retval - esp specific error codes
*/
int bmp_i2c_set_mode(bmp_handle_t bmp, bmp_mode_t mode);

/**
 * @brief Set standby time of BMP280 device in normal mode.
 * 
 * @param bmp BMP device handle.
 * @param time One of possible times. ex. BMP_STANDBY_1000M
 * 
 * @retval - 0 on success
 * @retval - ERR_NULL_POINTER - when BMP device handle is NULL.
 * @retval - esp specific error codes
*/
int bmp_i2c_set_standby_time(bmp_handle_t bmp, bmp_stdby_time_t time);

/**
 * @brief Set pressure oversampling.
 * 
 * @param bmp BMP device handle.
 * @param oversampling One of possible oversampling values, starting with BMP_OVERSAMPLING_...
 * 
 * @retval - 0 on success
 * @retval - ERR_NULL_POINTER - when BMP device handle is NULL.
 * @retval - esp specific error codes
*/
int bmp_i2c_set_press_oversampling(bmp_handle_t bmp, bmp_oversampling_t oversampling);

/**
 * @brief Set temperature oversampling.
 * 
 * @param bmp BMP device handle.
 * @param oversampling One of possible oversampling values, starting with BMP_OVERSAMPLING_...
 * 
 * @retval - 0 on success
 * @retval - ERR_NULL_POINTER - when BMP device handle is NULL.
 * @retval - esp specific error codes
*/
int bmp_i2c_set_temp_oversampling(bmp_handle_t bmp, bmp_oversampling_t oversampling);

/**
 * @brief Set IIR filter value.
 * 
 * @param bmp BMP device handle.
 * @param filter One of possible oversampling values, starting with BMP_IIR_...
 * 
 * @retval - 0 on success
 * @retval - ERR_NULL_POINTER - when BMP device handle is NULL.
 * @retval - esp specific error codes
*/
int bmp_i2c_set_iir_filter(bmp_handle_t bmp, bmp_iir_filter_t filter);

/**
 * @brief Read BMP calibration data.
 * 
 * @param bmp BMP device handle.
 * 
 * @retval - 0 on success
 * @retval - ERR_NULL_POINTER - when BMP device handle is NULL.
 * @retval - esp specific error codes
*/
int bmp_i2c_calibrate(bmp_handle_t bmp);

/**
 * @brief Configure BMP280 device.
 * 
 * @param bmp BMP device handle.
 * @param config BMP device config.
 * 
 * @retval - 0 on success
 * @retval - i2c_controller specific error codes
 * @retval - esp specific error codes
*/
int bmp_configure(bmp_handle_t bmp, bmp_config_t* config);

/**
 * @brief Read BMP device pressure and temperature readings.
 * 
 * @param bmp BMP device handle.
 * @param temperature[out] Buffer to store temperature.
 * @param pressure[out] Buffer to store pressure.
 * 
 * @retval - 0 on success
 * @retval - ERR_NULL_POINTER - when any of the arguments is NULL.
 * @retval - esp specific error codes
*/
int bmp_i2c_readout(bmp_handle_t bmp, int32_t* temperature, uint32_t* pressure);

/**
 * @brief Get calculated altitude based on pressure and pressure on sea level.
 * 
 * @param pressure Pressure in hPa
 * @param sea_level_hPa Pressure at sea level in hPa
 * 
 * @return Altitude in metres
*/
float bmp_i2c_calculate_altitude(float pressure, float sea_level_hPa);

/**
 * @brief Convert sensor readout to floating point values.
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
 * @brief Read sensor values as floating point numbers.
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