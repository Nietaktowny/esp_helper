idf_component_register(
    SRCS "src/wifi_controller.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES err_controller logger memory_utils esp_wifi)
