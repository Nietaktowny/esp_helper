#pragma once

/**
 * @file adc_controller.h
 * @author Wojciech Mytych
 * @brief ADC Controller header file.
 * @version 1.0
 * @date 2024-04-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "adc_c_errors.h"

struct adc_c_oneshot_handle_obj;

typedef struct adc_c_oneshot_handle_obj *adc_c_oneshot_handle_t;

/**
 * @brief ADC channels
 */
typedef enum
{
    ADC_C_CHANNEL_0, ///< ADC channel 0
    ADC_C_CHANNEL_1, ///< ADC channel 1
    ADC_C_CHANNEL_2, ///< ADC channel 2
    ADC_C_CHANNEL_3, ///< ADC channel 3
    ADC_C_CHANNEL_4, ///< ADC channel 4
    ADC_C_CHANNEL_5, ///< ADC channel 5
    ADC_C_CHANNEL_6, ///< ADC channel 6
    ADC_C_CHANNEL_7, ///< ADC channel 7
    ADC_C_CHANNEL_8, ///< ADC channel 8
    ADC_C_CHANNEL_9, ///< ADC channel 9
} adc_c_channel_t;

typedef enum
{
/**
 * @brief Definitions of ADC supporting GPIO for ESP32-C3 boards..
 *
 */
#ifdef ESP32_C3_SUPERMINI
    ADC_C_GPIO_0 = 0, ///< ADC GPIO number 0
    ADC_C_GPIO_1,     ///< ADC GPIO number 1
    ADC_C_GPIO_2,     ///< ADC GPIO number 2
    ADC_C_GPIO_3,     ///< ADC GPIO number 3
    ADC_C_GPIO_4      ///< ADC GPIO number 4
/**
 * @brief Definitions of ADC supporting GPIO for ESP32 boards.
 *
 */
#else
    ADC_C_GPIO_2 = 2,   ///< ADC GPIO number 2
    ADC_C__GPIO_4 = 4,  ///< ADC GPIO number 4
    ADC_C_GPIO_12 = 12, ///< ADC GPIO number 12
    ADC_C_GPIO_13,      ///< ADC GPIO number 13
    ADC_C_GPIO_14,      ///< ADC GPIO number 14
    ADC_C_GPIO_15,      ///< ADC GPIO number 15
    ADC_C_GPIO_25 = 25, ///< ADC GPIO number 25
    ADC_C_GPIO_26,      ///< ADC GPIO number 26
    ADC_C_GPIO_27,      ///< ADC GPIO number 27
    ADC_C_GPIO_32 = 32, ///< ADC GPIO number 32 
    ADC_C_GPIO_33,      ///< ADC GPIO number 33
    ADC_C_GPIO_34,      ///< ADC GPIO number 34
    ADC_C_GPIO_35,      ///< ADC GPIO number 35
    ADC_C_GPIO_36,      ///< ADC GPIO number 36 
    ADC_C_GPIO_39 = 39, ///< ADC GPIO number 39
#endif // ESP32_C3_SUPERMINI
} adc_c_gpio_t;         ///< ADC GPIO number definition type.

/**
 * @brief Initialize ADC UNIT and specified GPIO.
 * 
 * Can be called multiple times for multiple GPIO-s.
 * 
 * @param handle Location to store ADC Handle. 
 * @param gpio ADC supporting GPIO.
 * 
 * @retval ERR_C_OK on success
 * @retval ADC_C_ERR_ADC_ALREADY_INIT on already inti ADC Unit.
 * @retval ADC_C_ERR_WRONG_GPIO  when GPIO doesn't support ADC or doesn't exist.
 * @retval ERR_C_NULL_POINTER   On null handle.
 * @retval esp specific errors
 */
int adc_c_init_oneshot(adc_c_oneshot_handle_t *handle, adc_c_gpio_t gpio);

/**
 * @brief Deinit ADC Controller instance. 
 * 
 * @param handle Created ADC Controller handle. 
 * @retval ERR_C_OK on success
 * @retval ERR_C_NULL_POINTER on NULL handle
 * @retval ADC_C_ERR_NOT_INIT when handle was not init.
 * @retval esp specific errors.
 */
int adc_c_deinit_oneshot(adc_c_oneshot_handle_t *handle);

/**
 * @brief Read ADC value from specified channel.
 * 
 * @param handle ADC Controller handle.
 * @param channel Channel to read from.
 * @param out_raw Location to store calibrated result.
 * 
 * @retval ERR_C_OK on success
 * @retval ERR_C_NULL_POINTER on NULL handle
 * @retval ADC_C_ERR_WRONG_GPIO when GPIO doesn't support ADC or doesn't exist.
 * @retval esp specific errors
 */
int adc_c_oneshot_read_channel(adc_c_oneshot_handle_t handle, adc_c_channel_t channel, int *out_raw);

/**
 * @brief Calibrate ADC Unit.
 * 
 * @param handle Created ADC Handle.
 *  
 * @retval ERR_C_OK on success
 * @retval ERR_C_NULL_POINTER on NULL handle
 * @retval esp specific errors
 */
int adc_c_oneshot_calibrate(adc_c_oneshot_handle_t handle);

/**
 * @brief Read ADC calibrated result from specified GPIO.
 * 
 * @param handle Created ADC Controller handle 
 * @param gpio ADC supporting GPIO
 * @param out Location to store calibrated result
 * @retval ERR_C_OK on success
 * @retval ERR_C_NULL_POINTER on null handle
 * @retval ADC_C_ERR_WRONG_GPIO when GPIO doesn't support ADC or doesn't exist
 * @retval esp specific errors
 */
int adc_c_oneshot_read_gpio(adc_c_oneshot_handle_t handle, adc_c_gpio_t gpio, int *out);
