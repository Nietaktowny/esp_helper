idf_component_register(
    SRCS "src/wifi_manager.c" "src/wifi_http_server.c" "src/wifi_http_server.h"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES wifi_controller esp_netif esp-tls esp_http_server nvs_controller nvs_flash spiffs_controller err_controller logger memory_utils)
