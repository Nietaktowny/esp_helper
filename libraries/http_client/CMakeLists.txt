idf_component_register(
    SRCS "src/http_client.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES esp_http_client esp-tls json arena err_controller logger memory_utils)
