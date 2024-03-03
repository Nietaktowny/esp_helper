#include <stddef.h>
#include "wifi_manager_errors.h"



int wifi_manager_init(void);

int wifi_manager_start_ap_and_server(void);

int wifi_manager_get_stored_ap(char* ssid, size_t ssid_len, char* password, size_t password_len);

/**
 * @brief Store access point credentials in Wifi Manager NVS namespace.
 * 
 * @param ssid SSID of access point to store. 
 * @param ssid_len Length of SSID.
 * @param password Password of access point to store.
 * @param password_len Length of password.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL ssid or password.
 * @retval NVS Controller specific errors.
 * 
 */
int wifi_manager_store_ap(const char* ssid, size_t ssid_len, const char* password, size_t password_len);

int wifi_manager_check_for_stored_ap(void);

int wifi_manager_erase_ap(void);
