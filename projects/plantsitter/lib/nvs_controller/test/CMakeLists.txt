idf_component_register(SRC_DIRS "."
                       INCLUDE_DIRS "."
                       REQUIRES unity nvs_controller
                       PRIV_REQUIRES nvs_flash err_controller logger memory_utils)