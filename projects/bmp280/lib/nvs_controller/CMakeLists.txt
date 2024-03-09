idf_component_register(
    SRCS "src/nvs_controller.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES nvs_flash err_controller logger memory_utils)
