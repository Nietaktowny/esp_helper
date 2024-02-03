#include "gpio_controller.h"
#include "err_controller.h"
#include "logger.h"

#include <driver/gpio.h>
#include <inttypes.h>



int gpio_c_set_level(gpio_c_number_t gpio, gpio_c_level_t level) {
    err_c_t err = 0;

    err = gpio_set_level(gpio, level);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d when trying to set gpio %d level to %d, error: %s", err, gpio, level, error_to_name(err));
    }

    return err;
}

int gpio_c_set_direction(gpio_c_number_t gpio, gpio_c_level_t direction) {
    err_c_t err = 0;

    err = gpio_set_direction(gpio, direction);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d when trying to set gpio %d direction to %d, error: %s", err, gpio, direction, error_to_name(err));
    }

    return err;
}