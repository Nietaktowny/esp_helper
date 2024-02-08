#include <inttypes.h>
#include "esp_bit_defs.h"

typedef enum{
    GPIO_C_LOW,
    GPIO_C_HIGH,
} gpio_c_level_t;

typedef enum{
    GPIO_C_MODE_DISABLE = 0,                                                             /*!< GPIO mode : disable input and output             */
    GPIO_C_MODE_INPUT = BIT0,                                                            /*!< GPIO mode : input only                           */
    GPIO_C_MODE_OUTPUT = BIT1,                                                           /*!< GPIO mode : output only mode                     */
    GPIO_C_MODE_OUTPUT_OD = ((GPIO_C_MODE_OUTPUT) | (BIT2)),                               /*!< GPIO mode : output only with open-drain mode     */
    GPIO_C_MODE_INPUT_OUTPUT_OD = ((GPIO_C_MODE_INPUT) | (GPIO_C_MODE_OUTPUT) | (BIT2)),     /*!< GPIO mode : output and input with open-drain mode*/
    GPIO_C_MODE_INPUT_OUTPUT = ((GPIO_C_MODE_INPUT) | (GPIO_C_MODE_OUTPUT)),                 /*!< GPIO mode : output and input mode                */
} gpio_c_direction_t;

typedef enum{
    GPIO_C_NUM_0,
    GPIO_C_NUM_1,
    GPIO_C_NUM_2,
    GPIO_C_NUM_3,
    GPIO_C_NUM_4,
    GPIO_C_NUM_5,
    GPIO_C_NUM_6,
    GPIO_C_NUM_7,
    GPIO_C_NUM_8,
    GPIO_C_NUM_9,
    GPIO_C_NUM_10,
    GPIO_C_NUM_11,
    GPIO_C_NUM_12,
    GPIO_C_NUM_13,
    GPIO_C_NUM_14,
    GPIO_C_NUM_15,
    GPIO_C_NUM_16,
    GPIO_C_NUM_17,
    GPIO_C_NUM_18,
    GPIO_C_NUM_19,
    GPIO_C_NUM_20,
    GPIO_C_NUM_21,
    GPIO_C_NUM_22,
    GPIO_C_NUM_23,
    GPIO_C_NUM_24,
    GPIO_C_NUM_25,
    GPIO_C_NUM_26,
    GPIO_C_NUM_27,
    GPIO_C_NUM_28,
    GPIO_C_NUM_29,
    GPIO_C_NUM_30,
    GPIO_C_NUM_31,
    GPIO_C_NUM_32,
    GPIO_C_NUM_33,
    GPIO_C_NUM_34,
    GPIO_C_NUM_35,
    GPIO_C_NUM_36,
    GPIO_C_NUM_37,
    GPIO_C_NUM_38,
    GPIO_C_NUM_39,
} gpio_c_number_t;


int gpio_c_set_level(gpio_c_number_t gpio, gpio_c_level_t level);

int gpio_c_set_direction(gpio_c_number_t gpio, gpio_c_level_t direction);