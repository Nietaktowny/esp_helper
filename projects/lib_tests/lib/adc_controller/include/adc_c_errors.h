#pragma once

/**
 * @file adc_c_errors.h
 * @author Wojciech Mytych
 * @brief ADC Controller errors header file.
 * @version 1.0
 * @date 2024-04-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#define ADC_C_ERR_BASE 0x04B0                              ///< ADC Controller error base.
#define ADC_C_ERR_ADC_ALREADY_INIT 0x0001 + ADC_C_ERR_BASE ///< This ADC unit is already init.
#define ADC_C_ERR_WRONG_GPIO 0x0002 + ADC_C_ERR_BASE       ///< The GPIO is not valid ADC pad.
#define ADC_C_ERR_TIMEOUT 0x0003 + ADC_C_ERR_BASE          ///< ADC timeout occured, result is not valid.
#define ADC_C_ERR_NOT_INIT 0x0004 + ADC_C_ERR_BASE         ///< ADC unit not initialized.
