#pragma once

#include <stdbool.h>
#include "esp_wifi.h"

/**
 * @brief Types of available WiFi modes.
 * 
 */
typedef enum {
    WIFI_C_MODE_STA,        /*Use WiFi as STA.*/
    WIFI_C_MODE_AP,         /*Use WiFi as AP.*/
    WIFI_C_MODE_APSTA,      /*Use WiFi as AP+STA.*/
    WIFI_C_NO_MODE          /*No mode currently set.*/
} wifi_c_mode_t;

/**
 * @brief Object showing and maintaining current status of wifi_controller.
 * 
 */
struct wifi_c_status_obj {
    bool wifi_initialized;
    bool netif_initialized;
    wifi_c_mode_t wifi_mode;
    bool even_loop_started;
    bool sta_started;
    bool ap_started;
    bool scan_done;
    bool sta_connected;
    char ip[20];
};

/**
 * @brief Type of wifi_controller status object.
 * 
 */
typedef struct wifi_c_status_obj wifi_c_status_t;


/**
 * @brief Object containing scanned AP records.
 * 
 */
struct wifi_c_ap_record_obj {
    uint8_t bssid[6];                     /**< MAC address of AP */
    uint8_t ssid[33];                     /**< SSID of AP */
    uint8_t channel;                      /**< channel of AP */
    int8_t  rssi;                         /**< signal strength of AP */
};

/**
 * @brief Type of AP scan result.
 * 
 */
typedef struct wifi_c_ap_record_obj wifi_c_ap_record_t;

/**
 * @brief Object representing scan configuration and results.
 * 
 */
struct wifi_c_scan_result_obj {
    wifi_c_ap_record_t* ap_record;
    uint16_t ap_count;
};

/**
 * @brief Type of scan results object.
 * 
 */
typedef struct wifi_c_scan_result_obj wifi_c_scan_result_t;

/**
 * @brief Definitions of error codes for wifi_controller.
 * 
 */
#define WIFI_C_ERR_BASE                 0x00FF                      ///< Error base used to indicate where wifi_controller errors numbers start.
#define WIFI_C_ERR_NULL_SSID            WIFI_C_ERR_BASE + 0x01      ///< SSID for WiFi was null or zero length.
#define WIFI_C_ERR_WRONG_MODE           WIFI_C_ERR_BASE + 0x02      ///< Mode type of WiFI was wrong.
#define WIFI_C_ERR_NETIF_INIT_FAILED    WIFI_C_ERR_BASE + 0x03      ///< Failed to initialize netif - see wifi_c_init_netif() (CRITICAL).
#define WIFI_C_ERR_WIFI_ALREADY_INIT    WIFI_C_ERR_BASE + 0x04      ///< WiFi was already initialized once.
#define WIFI_C_ERR_NETIF_ALREADY_INIT   WIFI_C_ERR_BASE + 0x05      ///< Netif is already initialized.
#define WIFI_C_ERR_WRONG_PASSWORD       WIFI_C_ERR_BASE + 0x06      ///< Password lenght is too short for WIFI_AUTH_WPA2_PSK (need at least 8 characters).
#define WIFI_C_ERR_WIFI_NOT_STARTED     WIFI_C_ERR_BASE + 0x07      ///< Wifi was not started.
#define WIFI_C_ERR_WIFI_NOT_INIT        WIFI_C_ERR_BASE + 0x08      ///< Wifi was not initialized.
#define WIFI_C_ERR_SCAN_NOT_DONE        WIFI_C_ERR_BASE + 0x09      ///< Trying to read scan results without prior scanning.
#define WIFI_C_ERR_STA_NOT_STARTED      WIFI_C_ERR_BASE + 0x0A      ///< Trying to scan without configuring nad starting STA.
#define WIFI_C_ERR_AP_NOT_FOUND         WIFI_C_ERR_BASE + 0x0B      ///< Not found desired AP when scanning.
#define WIFI_C_ERR_NEITF_NOT_INIT       WIFI_C_ERR_BASE + 0x0C      ///< Netif was not initialized.
#define WIFI_C_ERR_EVENT_LOOP_NOT_INIT  WIFI_C_ERR_BASE + 0x0D      ///< Event loop was not started.
#define WIFI_C_ERR_STA_NOT_CONNECTED    WIFI_C_ERR_BASE + 0x0E      ///< STA is not connected to any AP.
#define WIFI_C_ERR_STA_CONNECT_FAIL     WIFI_C_ERR_BASE + 0x0F      ///< STA failed to connect to AP.
#define WIFI_C_ERR_STA_TIMEOUT_EXPIRE   WIFI_C_ERR_BASE + 0x10      ///< wifi_c_start_sta function timeout expired, returned without connection to WiFi


#define WIFI_C_STA_RETRY_COUNT          4                           ///< Number of times to try to connect to AP as STA.
#define WIFI_C_DEFAULT_SCAN_SIZE        10                          ///< Number of APs to store when scanning.
#define WIFI_C_STA_TIMEOUT              60                          ///< Number of seconds for which will wifi_c_start_sta will block before returning

#define WIFI_C_CONNECTED_BIT            0x00000001
#define WIFI_C_CONNECT_FAIL_BIT         0x00000002
#define WIFI_C_SCAN_DONE_BIT            0x00000004
#define WIFI_C_STA_STARTED_BIT          0x00000008

#define WIFI_C_SCAN_BLOCK               true                        ///< if block is true, this API will block the caller until the scan is done

/**
 * @brief Used to initialize and prepare Wifi to work.
 * 
 * @param WIFI_C_WIFI_MODE Mode in which WiFi will work.
 * @return
 *          - ERR_C_OK on success
 *          - WIFI_C_ERR_WIFI_ALREADY_INIT if already initialized
 *          - esp specific errors
 */
int wifi_c_init_wifi(wifi_c_mode_t WIFI_C_WIFI_MODE);

/**
 * @brief Starts WiFi in softAP mode.
 * 
 * @attention
 * Passed SSID of AP cannot be zero length otherwise it will throw error.
 * 
 * @note
 * When passed password length is zero, the auth mode is set to open.
 * 
 * @param ssid          SSID of AP.
 * @param password      Password of AP.
 * @return
 *          - ERR_C_OK on success
 *          - WIFI_C_ERR_NULL_SSID if passed ssid was null or zero length
 *          - ERR_C_MEMORY_ERR if memcpy of password/ssid was not successfull
 *          - esp specific error codes
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
 * @brief Get current IPv4 address of device.
 * 
 * @retval IPv4 address
 * @retval 0.0.0.0 if no address was currently received
*/
char* wifi_c_get_ipv4(void);

/**
 * @brief Get Wifi STA connection status.
 * 
 * @return true If STA is connected to some AP.
 * @return false If STA is not connected to any AP.
 */
bool wifi_c_check_if_sta_is_connected(void);

/**
 * @brief Initializes default event loop and sets callback functions.
 * 
 * @return
 *          - ERR_C_OK on success
 *          - esp specific error codes
 */
int wifi_c_create_default_event_loop(void);

/**
 * @brief Disconnect from AP as STA.
 * 
 * @return
 *          - ERR_C_OK on success
 *          - WIFI_C_ERR_WIFI_NOT_INIT
 *          - WIFI_C_ERR_WIFI_NOT_STARTED
 *          - WIFI_C_ERR_STA_NOT_STARTED
 *          - WIFI_C_ERR_STA_NOT_CONNECTED
 *          - esp specific error codes
 */
int wifi_c_disconnect(void);

/**
 * @brief Disconnect from sta, and try to connect with passed credentials.
 * 
 * @param SSID SSID to AP;
 * @param PASSWORD Password to AP.
 * @return
 *          - ERR_C_OK on success
 *          - WIFI_C_ERR_WIFI_NOT_INIT
 *          - WIFI_C_ERR_WIFI_NOT_STARTED
 *          - WIFI_C_ERR_STA_NOT_STARTED
 *          - WIFI_C_ERR_STA_NOT_CONNECTED
 *          - esp specific error codes
 */
int wifi_c_sta_reconnect(const char* SSID, const char* PASSWORD);

/**
 * @brief Scan for AP on all channels.
 * 
 * @param result_to_return Pointer to scan results struct.
 * @return
 *          - ERR_C_OK on success
 *          - WIFI_C_ERR_WRONG_MODE Wrong Wifi mode, scanning only possible in STA/APSTA mode.
 *          - WIFI_C_ERR_WIFI_NOT_INIT WiFi was not initialized.
 *          - WIFI_C_ERR_STA_NOT_STARTED STA was not started.
 *          - esp specific error codes
 */
int wifi_c_scan_all_ap(wifi_c_scan_result_t* result_to_return);

/**
 * @brief Scan for AP with desired SSID.
 * 
 * @param searched_ssid SSID of AP to search for.
 * @param ap_record     Pointer to wifi_ap_record_t to store result.
 * @return
 *          - ERR_C_OK on success
 *          - WIFI_C_ERR_AP_NOT_FOUND when not found AP
 *          - esp specific error codes
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
 * @brief Store results of scanning in buffer as string.
 * 
 * @param buffer Buffer to store scan result.
 * @param buflen Length of the buffer.
 * @return
 *          - ERR_C_OK on success
 *          - WIFI_C_ERR_SCAN_NOT_DONE Scan not done, init scan before getting results.
 *          - WIFI_C_ERR_WIFI_NOT_INIT WiFi was not initialized.
 *          - esp specific error codes
 */
int wifi_c_store_scanned_ap (char buffer[], uint16_t buflen);

/**
 * @brief Used to deinit wifi controller, and free all resources.
 * 
 * @note This function deletes default event loop.
 */
void wifi_c_deinit(void);


void wifi_c_sta_register_disconnect_handler(void (*disconnect_fun_ptr)(void));