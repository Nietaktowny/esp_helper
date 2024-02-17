idf_component_register(
    SRCS "src/i2c_controller.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES driver arena err_controller logger memory_utils)