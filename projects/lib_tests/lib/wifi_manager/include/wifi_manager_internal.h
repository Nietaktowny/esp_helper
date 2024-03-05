/**
 * @file wifi_manager_internal.h
 * @brief  Wifi manager library internal header file.
 * @author Wojciech Mytych
 * @version 1.1.4
 * @date 2024-03-05
 */



#include <stddef.h>


/* -------------------------------*/
/**
 * @brief  Wifi manager server handle object.
 */
/* ---------------------------------*/
struct wifi_s_handle_obj;


/* -------------------------------*/
/**
 * @brief  Wifi manager server handle.
 */
/* ---------------------------------*/
typedef struct wifi_s_handle_obj* wifi_s_handle_t;

int wifi_manager_server_init(wifi_s_handle_t* out_handle);

int wifi_manager_get_scanned_aps(char* out_buffer, size_t buflen);

int wifi_manager_get_stored_ap_as_json(char* buffer, size_t bufflen);
