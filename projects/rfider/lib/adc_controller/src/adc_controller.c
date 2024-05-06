/**
 * @file adc_controller.c
 * @author Wojciech Mytych
 * @brief ADC Controller source file.
 * @version 1.0
 * @date 2024-04-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "adc_controller.h"
#include "err_controller.h"
#include "logger.h"
#include "memory_utils.h"

#ifdef ESP_PLATFORM

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"

struct adc_c_oneshot_handle_obj {
  adc_oneshot_unit_handle_t esp_handle;
  adc_oneshot_unit_init_cfg_t esp_config;
  adc_cali_handle_t esp_cali;
};

static inline char *adc_c_translate_channel(adc_c_channel_t channel) {
  switch (channel) {
  case ADC_C_CHANNEL_0:
    return "ADC_C_CHANNEL_0";
  case ADC_C_CHANNEL_1:
    return "ADC_C_CHANNEL_1";
  case ADC_C_CHANNEL_2:
    return "ADC_C_CHANNEL_2";
  case ADC_C_CHANNEL_3:
    return "ADC_C_CHANNEL_3";
  case ADC_C_CHANNEL_4:
    return "ADC_C_CHANNEL_4";
  case ADC_C_CHANNEL_5:
    return "ADC_C_CHANNEL_5";
  case ADC_C_CHANNEL_6:
    return "ADC_C_CHANNEL_6";
  case ADC_C_CHANNEL_7:
    return "ADC_C_CHANNEL_7";
  case ADC_C_CHANNEL_8:
    return "ADC_C_CHANNEL_8";
  case ADC_C_CHANNEL_9:
    return "ADC_C_CHANNEL_9";
  default:
    break;
  }

  return "not known channel";
}

int adc_c_init_oneshot(adc_c_oneshot_handle_t *handle, adc_c_gpio_t gpio) {
  err_c_t err = ERR_C_OK;
  adc_channel_t channel = -1;
  ERR_C_CHECK_NULL_PTR(handle,
                       LOG_ERROR("location to store handle cannot be NULL"));

  NEW(*handle, struct adc_c_oneshot_handle_obj);

  /*
   * if number of ADC gpio is bigger than number of all GPIOS in chip
   * then adc_oneshot_io_to_channel return ESP_ERR_INVALID_ARG instead of
   * ESP_ERR_NOT_FOUND manualy check it and return correct error
   */
  // translate gpio to channel
  err = adc_oneshot_io_to_channel(gpio, &((*handle)->esp_config.unit_id),
                                  &channel);
  if ((err != ERR_C_OK && err == ESP_ERR_NOT_FOUND) ||
      gpio >= SOC_GPIO_PIN_COUNT) {
    LOG_ERROR("error %d, passed GPIO %d is not a valid ADC pad.", err, gpio);
    return ADC_C_ERR_WRONG_GPIO;
  } else if (err != ERR_C_OK) {
    LOG_ERROR("error %d when translating GPIO %d to channel: %s", err, gpio,
              error_to_name(err));
    return ERR_C_WRONG_ARGS;
  }

  (*handle)->esp_config.ulp_mode = ADC_ULP_MODE_DISABLE;
  (*handle)->esp_config.clk_src = 0;

  err =
      adc_oneshot_new_unit(&((*handle)->esp_config), &((*handle)->esp_handle));
  if (err != ERR_C_OK && err == ESP_ERR_NOT_FOUND) {
    err = ADC_C_ERR_ADC_ALREADY_INIT;
    LOG_WARN("error %d, the ADC unit %d is already in use.", err,
             (*handle)->esp_config.unit_id);
  } else if (err != ERR_C_OK) {
    LOG_ERROR("error %d when trying to init oneshot adc: %s", err,
              error_to_name(err));
    memutil_zero_memory(*handle, sizeof(struct adc_c_oneshot_handle_obj));
    DELETE(*handle);
    return err;
  }

  adc_oneshot_chan_cfg_t channel_config = {
      .bitwidth = ADC_BITWIDTH_DEFAULT,
      .atten = ADC_ATTEN_DB_11,
  };

  ERR_C_CHECK_ERROR(
      adc_oneshot_config_channel((*handle)->esp_handle, channel,
                                 &channel_config),
      LOG_ERROR("error %d when trying trying to config adc channel: %s", err,
                error_to_name(err)));

  ERR_C_CHECK_ERROR(adc_c_oneshot_calibrate(*handle),
                    LOG_ERROR("error %d when calibrating ADC channel %s: %s",
                              err, adc_c_translate_channel(channel),
                              error_to_name(err)));

  return err;
}

int adc_c_deinit_oneshot(adc_c_oneshot_handle_t *handle) {
  err_c_t err = ERR_C_OK;
  ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("handle cannot be NULL"));
  ERR_C_CHECK_NULL_PTR(*handle,
                       LOG_ERROR("handle was not initalized, cannot be NULL"));
  err = adc_oneshot_del_unit((*handle)->esp_handle);
  if (err != ERR_C_OK) {
    LOG_ERROR("error %d during oneshot adc handle deinit: %s", err,
              error_to_name(err));
  }

#ifdef ESP32_C3_SUPERMINI
  adc_cali_delete_scheme_curve_fitting((*handle)->esp_cali);
#else
  adc_cali_delete_scheme_line_fitting((*handle)->esp_cali);
#endif

  if (err != ERR_C_OK && err == ESP_ERR_NOT_FOUND) {
    err = ADC_C_ERR_NOT_INIT;
    LOG_ERROR("error %d, the ADC peripheral to be disclaimed isn't in use.",
              err);
  } else if (err != ERR_C_OK) {
    LOG_ERROR("error %d during oneshot calibration handle deinit: %s", err,
              error_to_name(err));
  }

  memutil_zero_memory(*handle, sizeof(struct adc_c_oneshot_handle_obj));
  DELETE(*handle);
  return err;
}

int adc_c_oneshot_calibrate(adc_c_oneshot_handle_t handle) {
  err_c_t err = ERR_C_OK;
  ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("ADC handle cannot be NULL"));
#ifdef ESP32_C3_SUPERMINI

  adc_cali_curve_fitting_config_t cali_config = {
      .unit_id = handle->esp_config.unit_id,
      .atten = ADC_ATTEN_DB_11,
      .bitwidth = ADC_BITWIDTH_DEFAULT,
  };

  ERR_C_CHECK_ERROR(
      adc_cali_create_scheme_curve_fitting(&cali_config, &((*handle).esp_cali)),
      LOG_ERROR("error %d when creating curve fitting calibration scheme: %s",
                err, error_to_name(err)));

  LOG_DEBUG("ADC calibration scheme version is Curve Fitting");

#else // FOR ESP32 chips
  adc_cali_line_fitting_config_t cali_config = {
      .unit_id = handle->esp_config.unit_id,
      .atten = ADC_ATTEN_DB_11,
      .bitwidth = ADC_BITWIDTH_DEFAULT,
  };

  ERR_C_CHECK_ERROR(
      adc_cali_create_scheme_line_fitting(&cali_config, &((*handle).esp_cali)),
      LOG_ERROR("error %d when creating line fitting calibration scheme: %s",
                err, error_to_name(err)));

  LOG_DEBUG("ADC calibration scheme version is Line Fitting");

#endif
  return err;
}

int adc_c_oneshot_read_channel(adc_c_oneshot_handle_t handle,
                               adc_c_channel_t channel, int *out) {
  err_c_t err = ERR_C_OK;
  int raw = -1;

  ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("ADC handle cannot be NULL"));
  ERR_C_CHECK_NULL_PTR(
      out, LOG_ERROR("location to store ADC result cannot be NULL"));

  err = adc_oneshot_read(handle->esp_handle, channel, &raw);
  if (err != ERR_C_OK && err == ESP_ERR_TIMEOUT) {
    LOG_WARN("error %d, cannot obtain result, timeout occured: %s", err,
             error_to_name(err));
    return err;
  } else if (err != ERR_C_OK && err != ESP_ERR_TIMEOUT) {
    LOG_ERROR("error %d during reading oneshot raw ADC result: %s", err,
              error_to_name(err));
    return err;
  }

  LOG_VERBOSE("read ADC raw value: %d from channel: %s", raw,
              adc_c_translate_channel(channel));

  // calculate voltage according to docs
  ERR_C_CHECK_ERROR(
      adc_cali_raw_to_voltage(handle->esp_cali, raw, out),
      LOG_ERROR("error %d when translating raw ADC values to voltage: %s", err,
                error_to_name(err)));

  // use now raw to store gpio number
  adc_oneshot_channel_to_io(ADC_UNIT_1, channel, &raw);
  LOG_VERBOSE("read voltage value %d [mV] from GPIO %d", *out, raw);
  return err;
}

int adc_c_oneshot_read_gpio(adc_c_oneshot_handle_t handle, adc_c_gpio_t gpio,
                            int *out) {
  volatile err_c_t err = ERR_C_OK;
  adc_channel_t channel = -1;

  ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("ADC handle cannot be NULL"));
  ERR_C_CHECK_NULL_PTR(
      out, LOG_ERROR("location to store ADC result cannot be NULL"));

  Try {
    /*
     * if number of ADC gpio is bigger than number of all GPIOS in chip
     * then adc_oneshot_io_to_channel return ESP_ERR_INVALID_ARG instead of
     * ESP_ERR_NOT_FOUND manualy check it and return correct error
     */
    if (gpio >= SOC_GPIO_PIN_COUNT) {
      ERR_C_SET_AND_THROW_ERR(err, ESP_ERR_NOT_FOUND);
    }
    // translate gpio to channel
    ERR_C_CHECK_AND_THROW_ERR(adc_oneshot_io_to_channel(
        gpio, &(handle->esp_config.unit_id), &channel));

    ERR_C_CHECK_AND_THROW_ERR(adc_oneshot_get_calibrated_result(
        handle->esp_handle, handle->esp_cali, channel, out));

    LOG_VERBOSE("read voltage value %d [mV] from GPIO %d", *out, gpio);
  }
  Catch(err) {
    switch (err) {
    case ESP_ERR_NOT_FOUND:
      err = ADC_C_ERR_WRONG_GPIO;
      break;
    case ESP_ERR_TIMEOUT:
      err = ADC_C_ERR_TIMEOUT;
      break;
    case ESP_ERR_INVALID_ARG:
      err = ERR_C_WRONG_ARGS;
      break;
    }
    LOG_ERROR("%s", error_to_name(err));
    *out = -1;
  };

  return err;
}

#endif // ESP_PLATFORM
