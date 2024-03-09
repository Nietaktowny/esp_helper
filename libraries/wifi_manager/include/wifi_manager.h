/**
 * @file wifi_manager.h
 * @brief  Wifi manager source file.
 * @author Wojciech Mytych
 * @version 1.1.7
 * @date 2024-03-05
 */

#include <stddef.h>
#include "wifi_manager_errors.h"

#define WIFI_MANAGER_NVS_NAMESPACE "wifi_manager"					///< Wifi manager namespace name.


/* -------------------------------*/
/**
 * @brief  Initialize wifi manager library.
 *
 * @retval   ERR_C_OK on success.
 */
/* ---------------------------------*/
int wifi_manager_init(void);

int wifi_manager_start_ap_and_server(void);


/* -------------------------------*/
/**
 * @brief  Get stored credentials of Wifi manager last stored access point.
 *
 * @param ssid Buffer to store access point SSID.
 * @param ssid_len SSID buffer length.
 * @param password Buffer to store access point password.
 * @param password_len Password buffer length.
 *
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL ssid or password.
 * @retval NVS_C_ERR_KEY_NOT_FOUND when not found any stored access point.
 */
/* ---------------------------------*/
int wifi_manager_get_stored_ap(char* ssid, size_t ssid_len, char* password, size_t password_len);

/**
 * @brief Store access point credentials in Wifi Manager NVS namespace.
 * 
 * @param ssid SSID of access point to store. 
 * @param ssid_len Length of SSID.
 * @param password Password of access point to store.
 * @param password length of password.
 * 
 * @retval ERR_C_OK on success.
 * @retval ERR_C_NULL_POINTER on NULL SSID or password.
 * @retval NVS Controller specific errors.
 * 
 */
int wifi_manager_store_ap(const char* ssid, size_t ssid_len, const char* password, size_t password_len);

int wifi_manager_check_for_stored_ap(void);


/* -------------------------------*/
/**
 * @brief  Erase all data stored in wifi manager NVS namespace.
 *
 * @retval ERR_C_OK on success.
 * @retval NVS Controller specific errors.
 */
/* ---------------------------------*/
int wifi_manager_erase_ap(void);
