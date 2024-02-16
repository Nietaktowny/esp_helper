idf_component_register(
    SRCS "src/spiffs_controller.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES spiffs err_controller logger memory_utils)
