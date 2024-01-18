#include <stddef.h>




int wifi_manager_init(void);

int wifi_manager_get_stored_ap(char* ssid, size_t ssid_len, char* password, size_t password_len);

int wifi_manager_store_ap(char* ssid, size_t ssid_len, char* password, size_t password_len);

int wifi_manager_check_for_stored_ap(void);