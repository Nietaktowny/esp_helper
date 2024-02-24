idf_component_register(
    SRCS "src/bmp280.c" "src/bmp280_internal.h"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES i2c_controller driver err_controller logger memory_utils)