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

/* -------------------------------*/
/**
 * @brief  Start wifi manager HTTP server.
 *
 * @param out_handle Location to store Wifi manager server handle.
 *
 * @retval   ERR_C_OK on success.
 * @retval	 ESP-IDF specific errors.
 */
/* ---------------------------------*/
int wifi_manager_server_init(wifi_s_handle_t* out_handle);

/* -------------------------------*/
/**
 * @brief  Scan and get for Access points that wifi manager can connect to.
 *
 * @param out_buffer Buffer to store scanned APs.
 * @param buflen Size of buffer.
 *
 * @retval   ERR_C_OK on success.
 * @retval   Wifi controller specific errors see @see wifi_c_scan_all_ap  
 */
/* ---------------------------------*/
int wifi_manager_get_scanned_aps(char* out_buffer, size_t buflen);


/* -------------------------------*/
/**
 * @brief  Scan and get for Access points that wifi manager can connect to and store them in JSON format.
 *
 * @param out_buffer Buffer to store scanned APs.
 * @param buflen Size of buffer.
 *
 * @retval   ERR_C_OK on success.
 * @retval   Wifi controller specific errors see @see wifi_c_scan_all_ap  
 */
/* ---------------------------------*/
int wifi_manager_get_stored_ap_as_json(char* buffer, size_t bufflen);
