idf_component_register(
    SRCS "src/ota_controller.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES http_client app_update esp_https_ota err_controller logger memory_utils)
