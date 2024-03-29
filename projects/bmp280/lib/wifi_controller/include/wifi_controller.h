/**
 * @file wifi_controller.h
 * @author Wojciech Mytych (wojciech.lukasz.mytych@gmail.com)
 * @brief Wifi controller header file.
 * @version 2.0.3
 * @date 2024-02-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include <stdbool.h>
#include <inttypes.h>
#include <stddef.h>
#include "wifi_c_errors.h"

/**
 * @brief Types of available WiFi modes.
 * 
 */
typedef enum {
    WIFI_C_NO_MODE,         /*!< No mode currently set.*/
    WIFI_C_MODE_STA,        /*!< Use WiFi as STA.*/
    WIFI_C_MODE_AP,         /*!< Use WiFi as AP.*/
    WIFI_C_MODE_APSTA,      /*!< Use WiFi as AP+STA.*/
} wifi_c_mode_t;

/**
 * @brief Object containing information about access point interface.
 * 
 */
struct wifi_c_ap_status_obj {
    char ip[20];                        /*!< IP of Access Point.*/
    char ssid[64];                      /*!< SSID of Access Point.*/
    void (*connect_handler)(void);      /*!< Function to be called each time new Station connects to Access Point.*/
    void (*disconnect_handler)(void);   /*!< Function to be called each time Station disconnects from Access Point.*/
};
/**
 * @brief Type of wifi_c_ap_status_obj object.
 * 
 */
typedef struct wifi_c_ap_status_obj wifi_c_ap_status_t;

/**
 * @brief Object containing information about station interface.
 * 
 */
struct wifi_c_sta_status_obj {
    char ip[20];                        /*!< Current IP of station, 0.0.0.0 if not connected to Wifi.*/
    char ssid[64];                      /*!< SSID of AP the STA is currently connected to.*/
    void (*connect_handler)(void);      /*!< Function to be called when STA connects to AP.*/
    void (*disconnect_handler)(void);   /*!< Function to be called when STA disconnects from AP.*/
};
/**
 * @brief Type of wifi_c_sta_status_obj object.
 * 
 */
typedef struct wifi_c_sta_status_obj wifi_c_sta_status_t;       

/**
 * @brief Object showing and maintaining current status of wifi_controller.
 * 
 */
struct wifi_c_status_obj {
    bool wifi_initialized;              /*!< True if wifi is initialized.*/
    bool netif_initialized;             /*!< True if netif interface is initialized.*/
    wifi_c_mode_t wifi_mode;            /*!< Wifi mode of wifi_controller.*/
    bool even_loop_started;             /*!< True if wifi event loop is started.*/
    bool sta_started;                   /*!< True if STA is started.*/
    bool ap_started;                    /*!< True if AP is started.*/
    bool scan_done;                     /*!< Set to true after first scan.*/
    bool sta_connected;                 /*!< True if STA is connected to some AP.*/
    wifi_c_sta_status_t sta;            /*!< STA information structure. @see wifi_c_sta_status_obj*/
    wifi_c_ap_status_t ap;              /*!< AP information structure.  @see wifi_c_ap_status_obj*/
};

/**
 * @brief Type of wifi_c_status_obj object.
 * 
 */
typedef struct wifi_c_status_obj wifi_c_status_t;


/**
 * @brief Object containing scanned AP record.
 * 
 */
struct wifi_c_ap_record_obj {
    uint8_t bssid[6];                     /*!< MAC address of AP */
    uint8_t ssid[33];                     /*!< SSID of AP */
    uint8_t channel;                      /*!< channel of AP */
    int8_t  rssi;                         /*!< signal strength of AP */
};

/**
 * @brief Type of AP wifi_c_ap_record_obj object..
 * 
 */
typedef struct wifi_c_ap_record_obj wifi_c_ap_record_t;

/**
 * @brief Object representing scan configuration and results.
 * 
 */
struct wifi_c_scan_result_obj {
    wifi_c_ap_record_t* ap_record;         /*!< Struct containing AP information, @see wifi_c_ap_record_t*/
    uint16_t ap_count;                     /*!< Number of scanned APs.*/
};

/**
 * @brief Type of wifi_c_scan_result_obj object.
 * 
 */
typedef struct wifi_c_scan_result_obj wifi_c_scan_result_t;


#define WIFI_C_STA_RETRY_COUNT          4                           ///< Number of times to try to connect to AP as STA.
#define WIFI_C_BUFFER_SCAN_SIZE         10                          ///< Max number of APs to store when scanning.
#define WIFI_C_STA_TIMEOUT              60                          ///< Number of seconds for which will wifi_c_start_sta will block before returning

#define WIFI_C_CONNECTED_BIT            0x00000001                  ///< Bit set if STA connected to AP.
#define WIFI_C_CONNECT_FAIL_BIT         0x00000002                  ///< Bit set if STA cannot connected to AP.
#define WIFI_C_SCAN_DONE_BIT            0x00000004                  ///< Bit set if STA finished scan.
#define WIFI_C_STA_STARTED_BIT          0x00000008                  ///< Bit set if STA is started.

#define WIFI_C_SCAN_BLOCK               true                        ///< if block is true, this API will block the caller until the scan is done

/**
 * @brief Used to initialize and prepare Wifi to work.
 * 
 * @param WIFI_C_WIFI_MODE Mode in which WiFi will work.
 * 
 * @retval ERR_C_OK on success
 * @retval WIFI_C_ERR_WIFI_ALREADY_INIT if already initialized
 * @retval esp specific errors
 */
int wifi_c_init_wifi(wifi_c_mode_t WIFI_C_WIFI_MODE);

/**
 * @brief Starts WiFi in softAP mode.
 * 
 * @attention
 * Passed SSID of AP cannot be zero length otherwise it will throw error.
 * 
 * @note
 * When passed password length is zero or NULL, the auth mode is set to open.
 * 
 * @param ssid          SSID of AP.
 * @param password      Password of AP.
 * 
 * @retval ERR_C_OK on success
 * @retval WIFI_C_ERR_NULL_SSID if passed ssid was null or zero length
 * @retval ERR_C_MEMORY_ERR if memcpy of password/ssid was not successfull
 * @retval esp specific error codes
 *          
 */
int wifi_c_start_ap(const char* ssid, const char* password);

/**
 * @brief Starts WiFi in STA mode.
 * 
 * @param ssid          SSID of AP to connect to as station.
 * @param password      password of AP to connect to as station.
 * 
 * @note This function will block for number of seconds specified by WIFI_C_STA_TIMEOUT before returning.
 * 
 * @retval ERR_C_OK on success
 * @retval WIFI_C_ERR_NULL_SSID if passed ssid was null or zero length
 * @retval ERR_C_MEMORY_ERR if memcpy of password/ssid was not successfull
 * @retval esp specific error codes
 */
int wifi_c_start_sta(const char* ssid, const char* password);

/**
 * @brief Get current wifi_controller status.
 * 
 * @return wifi_status_t* Pointer to wifi_controller status struct.
 */
wifi_c_status_t *wifi_c_get_status(void);


/**
 * @brief Get current wifi_controller status as JSON string.
 * 
 * @attention If buffer is to small to contain whole structure, the status
 * will be truncated to fit the buffer size.
 * 
 * Advised size is around 300 characters.
 * 
 * 
 * @param buffer Buffer to store wifi_c_status data.
 * @param buflen Size of buffer.
 * 
 * @retval ERR_C_NULL_POINTER if buffer is NULL. 
 * @retval ERR_C_MEMORY_ERR if buffer cannot contain structure.
 * 
*/
int wifi_c_get_status_as_json(char* buffer, size_t buflen);


/**
 * @brief Translate wifi_c_mode_t enum to string.
 * 
 * @param wifi_mode Wifi mode enum to translate.
 * 
 * @retval "WIFI_C_MODE_AP"
 * @retval "WIFI_C_MODE_STA"
 * @retval "WIFI_C_MODE_APSTA"
 * @retval "WIFI_C_NO_MODE"
 * @retval NULL if wrong value was passed.
*/
char* wifi_c_get_wifi_mode_as_string(wifi_c_mode_t wifi_mode);

/**
 * @brief Get current IPv4 address of STA interface.
 * 
 * @retval IPv4 address
 * @retval 0.0.0.0 if no address was currently received
*/
char* wifi_c_get_sta_ipv4(void);

/**
 * @brief Get current IPv4 address of AP interface.
 * 
 * @retval IPv4 address
 * @retval 0.0.0.0 if no address
*/
char* wifi_c_get_ap_ipv4(void);

/**
 * @brief Get SSID of access point that STA interface is connected to.
 * 
 * @retval SSID of access point
 * @retval "none" if STA is not connected to any access point
*/
char* wifi_c_sta_get_ap_ssid(void);

/**
 * @brief Get SSID of access point interface.
 * 
 * @retval SSID of access point
 * @retval "none" if AP is not started
*/
char* wifi_c_ap_get_ssid(void);

/**
 * @brief Get Wifi STA connection status.
 * 
 * @retval true If STA is connected to some AP.
 * @retval false If STA is not connected to any AP.
 * @retval true If STA is connected to some AP.
 * @retval false If STA is not connected to any AP.
 */
bool wifi_c_check_if_sta_is_connected(void);

/**
 * @brief Initializes default event loop and sets callback functions.
 * 
 * @retval ERR_C_OK on success
 * @retval esp specific error codes
 * @retval ERR_C_OK on success
 * @retval esp specific error codes
 */
int wifi_c_create_default_event_loop(void);

/**
 * @brief Disconnect from AP as STA.
 * 
 * @retval ERR_C_OK on success
 * @retval WIFI_C_ERR_WIFI_NOT_INIT
 * @retval WIFI_C_ERR_WIFI_NOT_STARTED
 * @retval WIFI_C_ERR_STA_NOT_STARTED
 * @retval WIFI_C_ERR_STA_NOT_CONNECTED
 * @retval esp specific error codes
 */
int wifi_c_disconnect(void);

/**
 * @brief Disconnect from sta, and try to connect with passed credentials.
 * 
 * @param SSID SSID to AP;
 * @param PASSWORD Password to AP.
 * 
 * @retval ERR_C_OK on success
 * @retval WIFI_C_ERR_WIFI_NOT_INIT
 * @retval WIFI_C_ERR_WIFI_NOT_STARTED
 * @retval WIFI_C_ERR_STA_NOT_STARTED
 * @retval WIFI_C_ERR_STA_NOT_CONNECTED
 * @retval esp specific error codes
 * 
 * @retval ERR_C_OK on success
 * @retval WIFI_C_ERR_WIFI_NOT_INIT
 * @retval WIFI_C_ERR_WIFI_NOT_STARTED
 * @retval WIFI_C_ERR_STA_NOT_STARTED
 * @retval WIFI_C_ERR_STA_NOT_CONNECTED
 * @retval esp specific error codes
 */
int wifi_c_sta_reconnect(const char* SSID, const char* PASSWORD);

/**
 * @brief Scan for AP on all channels.
 * 
 * @param result_to_return Pointer to scan results struct.
 * 
 * @attention Scanning for access points is only possible when station mode is enabled and started.
 * 
 * @retval ERR_C_OK on success
 * @retval WIFI_C_ERR_WRONG_MODE Wrong Wifi mode, scanning only possible in STA/APSTA mode.
 * @retval WIFI_C_ERR_WIFI_NOT_INIT WiFi was not initialized.
 * @retval WIFI_C_ERR_STA_NOT_STARTED STA was not started.
 * @retval ERR_C_NULL_POINTER Pointer to result buffer was NULL.
 * @retval esp specific error codes
 */
int wifi_c_scan_all_ap(wifi_c_scan_result_t** result_to_return);

/**
 * @brief Scan for AP with desired SSID.
 * 
 * @param searched_ssid SSID of AP to search for.
 * @param ap_record     Pointer to wifi_c_ap_record_t to store result.
 * 
 * @retval ERR_C_OK on success
 * @retval WIFI_C_ERR_AP_NOT_FOUND when not found AP
 * @retval esp specific error codes
 * 
 * @retval ERR_C_OK on success
 * @retval WIFI_C_ERR_AP_NOT_FOUND when not found AP
 * @retval esp specific error codes
 */
int wifi_c_scan_for_ap_with_ssid(const char* searched_ssid, wifi_c_ap_record_t* ap_record);

/**
 * @brief Log results of Wifi scan.
 * 
 * @note It should be called only after scanning is done.
 * 
 */
int wifi_c_print_scanned_ap (void);

/**
 * @brief Store results of scanning in buffer as json string;
 * 
 * @param buffer Buffer to store scan result.
 * @param buflen Length of the buffer.
 * 
 * @retval ERR_C_OK on success
 * @retval WIFI_C_ERR_SCAN_NOT_DONE Scan not done, init scan before getting results.
 * @retval WIFI_C_ERR_WIFI_NOT_INIT WiFi was not initialized.
 * @retval esp specific error codes
 * 
 * @retval ERR_C_OK on success
 * @retval WIFI_C_ERR_SCAN_NOT_DONE Scan not done, init scan before getting results.
 * @retval WIFI_C_ERR_WIFI_NOT_INIT WiFi was not initialized.
 * @retval esp specific error codes
 */
int wifi_c_store_scan_result_as_json (char* buffer, uint16_t buflen);

/**
 * @brief Change wifi operating mode.
 * 
 * @param mode wifi operating mode (STA, AP, APSTA)
 * 
 * @warning This function destroys and reinitializes state of wifi_controller
 * 
 * @retval WIFI_C_ERR_WRONG_MODE If mode is the same as currently set.
 * @retval esp specific error codes
*/
int wifi_c_change_mode(wifi_c_mode_t mode);

/**
 * @brief Used to deinit wifi controller, and free all resources.
 * 
 * @note This function deletes default event loop.
 */
void wifi_c_deinit(void);

/**
 * @brief Register function to be called when STA connects to AP.
 * 
 * @param connect_handler Pointer to handler function.
 * 
 * @retval 0 on success
 * @retval ERR_C_NULL_POINTER when connect_handler function pointer is NULL
*/
int wifi_c_sta_register_connect_handler(void (*connect_handler)(void));

/**
 * @brief Register function to be called each time new STA connects to AP.
 * 
 * @param connect_handler Pointer to handler function.
 * 
 * @retval 0 on success
 * @retval ERR_C_NULL_POINTER when connect_handler function pointer is NULL
*/
int wifi_c_ap_register_connect_handler(void (*connect_handler)(void));

/**
 * @brief Register function to be called when STA loses connection with AP and all attempts to connect again fails.
 * 
 * @param connect_handler Pointer to handler function.
 * 
 * @retval 0 on success
 * @retval ERR_C_NULL_POINTER when disconnect_handler function pointer is NULL
*/
int wifi_c_sta_register_disconnect_handler(void (*disconnect_handler)(void));

/**
 * @brief Register function to be called each time STA disconnects from our AP.
 * 
 * @param connect_handler Pointer to handler function.
 * 
 * @retval 0 on success
 * @retval ERR_C_NULL_POINTER when disconnect_handler function pointer is NULL
*/
int wifi_c_ap_register_disconnect_handler(void (*disconnect_handler)(void));
