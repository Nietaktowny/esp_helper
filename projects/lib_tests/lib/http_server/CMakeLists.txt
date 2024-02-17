idf_component_register(
    SRCS "src/http_server.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES esp_http_server nvs_flash esp_netif esp-tls linked_list err_controller logger memory_utils)
