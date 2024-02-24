#pragma once
#include <stdint.h>
#include <stddef.h>


#define BMP_REG_CHIP_ID              UINT8_C(0xD0)      ///< Register address for chip identification number.
#define BMP_REG_RESET                UINT8_C(0xE0)      ///< Register address for sensor reset.
#define BMP_REG_STATUS               UINT8_C(0xF3)      ///< Register address of sensor status.
#define BMP_REG_CTRL_MEAS            UINT8_C(0xF4)      ///< Register address of sensor measurement control.
#define BMP_REG_CONFIG               UINT8_C(0xF5)      ///< Register address of sensor configuration.
#define BMP_REG_PRESS_MSB            UINT8_C(0xF7)      ///< Register address of pressure most significant byte.
#define BMP_REG_PRESS_LSB            UINT8_C(0xF8)      ///< Register address of pressure least significant byte.
#define BMP_REG_PRESS_XLSB           UINT8_C(0xF9)      ///< Register address of pressure fraction significant byte.
#define BMP_REG_TEMP_MSB             UINT8_C(0xFA)      ///< Register address of temperature most significant byte.
#define BMP_REG_TEMP_LSB             UINT8_C(0xFB)      ///< Register address of temperature least significant byte.
#define BMP_REG_TEMP_XLSB            UINT8_C(0xFC)      ///< Register address of temperature fraction significant byte.
#define BMP_REG_CAL_HI               UINT8_C(0xA1)      ///< Register address of calibration constants. (high bank)
#define BMP_REG_CAL_LOW              UINT8_C(0x88)      ///< Register address of calibration constants. (low bank)

#define BMP_CHIP_ID_FIRST            UINT8_C(0x56)      ///< BMP280 chip ID (first type)
#define BMP_CHIP_ID_SECOND           UINT8_C(0x57)      ///< BMP280 chip ID (second type)
#define BMP_CHIP_ID_THIRD            UINT8_C(0x58)      ///< BMP280 chip ID (third type)
#define BME_CHIP_ID                  UINT8_C(0x60)      ///< BME280 chip ID

#define BMP_RESET_VECTOR             UINT8_C(0xB6)      ///< Chip reset vector.
#define BMP_STARTUP_TIME             UINT8_C(2)         ///< Chip startup time after reset.

#define BMP_I2C_ADDR_HI              UINT8_C(0xEE)      ///< I2C chip address when SDO pin is high.
#define BMP_I2C_ADDR_LO              UINT8_C(0xEC)      ///< I2C chip address when SDO pin is low.

int32_t bmp_compensate_T_int32(bmp_handle_t bmp, int32_t adc_T);

uint32_t bmp_compensate_P_int64(bmp_handle_t bmp, int32_t adc_P);