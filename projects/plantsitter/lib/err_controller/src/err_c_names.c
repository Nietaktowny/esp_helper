/**
 * @file err_c_names.c
 * @author Wojciech Mytych
 * @brief Auto generated file with ESP Helper libraries error names.
 * @version 1.0.3
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "err_controller.h"
#include <string.h>

#ifdef ESP_PLATFORM
#include <esp_err.h>
#else
#include <errno.h>
#endif

const char* error_to_name(err_c_t err) {
    switch (err)
    {
	case 0x0001 + 0x0320:
		return "Cannot obtain device lock.";
	case 0x0000 + 0x0000:
		return "No error.";
	case 0x0001 + 0x0000:
		return "Null pointer was passed or dereferenced.";
	case 0x0002 + 0x0000:
		return "Memory allocation failed.";
	case 0x0003 + 0x0000:
		return "Memory error occurred.";
	case 0x0004 + 0x0000:
		return "Wrong arguments were used.";
	case 0x0001 + 0x0064:
		return "When some error with logger mutex occurred.";
	case 0x0002 + 0x0064:
		return "When function getting semaphore timeout expired and cannot obtain log_mutex.";
	case 0x0003 + 0x0064:
		return "When null pointer is passed as log file output.";
	case 0x0004 + 0x0064:
		return "When there are no new slots for log files, max number of files has been reached.";
	case 0x0001 + 0x044C:
		return "No BMP280 device was found on bus.";
	case 0x0002 + 0x044C:
		return "Cannot change BMP device operating mode.";
	case 0x0001 + 0x0190:
		return "Not enough space to store filepath.";
	case 0x0001 + 0x02BC:
		return "Error when searching for item in linked list.";
	case 0x0001 + 0x0384:
		return "When trying to perform OTA when STA is not connected.";
	case 0x0000-1 + 0x012C:
		return "Invalid socket identifier.";
	case 0x0023 + 0x012C:
		return "Error when preparing address.";
	case 0x0024 + 0x012C:
		return "Invalid port number.";
	case 0x0025 + 0x012C:
		return "Socket is invalid.";
	case 0x0026 + 0x012C:
		return "Wrong arguments passed to function.";
	case 0x0001 + 0x01F4:
		return "New and old image are the same.";
	case 0x01 + 0x00C8:
		return "SSID for WiFi was null or zero length.";
	case 0x02 + 0x00C8:
		return "Mode type of WiFI was wrong.";
	case 0x03 + 0x00C8:
		return "Failed to initialize netif - see wifi_c_init_netif() (CRITICAL).";
	case 0x04 + 0x00C8:
		return "WiFi was already initialized once.";
	case 0x05 + 0x00C8:
		return "Netif is already initialized.";
	case 0x06 + 0x00C8:
		return "Password lenght is too short for WIFI_AUTH_WPA2_PSK (need at least 8 characters).";
	case 0x07 + 0x00C8:
		return "Wifi was not started.";
	case 0x08 + 0x00C8:
		return "Wifi was not initialized.";
	case 0x09 + 0x00C8:
		return "Trying to read scan results without prior scanning.";
	case 0x0A + 0x00C8:
		return "Trying to scan without configuring nad starting STA.";
	case 0x0B + 0x00C8:
		return "Not found desired AP when scanning.";
	case 0x0C + 0x00C8:
		return "Netif was not initialized.";
	case 0x0D + 0x00C8:
		return "Event loop was not started.";
	case 0x0E + 0x00C8:
		return "STA is not connected to any AP.";
	case 0x0F + 0x00C8:
		return "STA failed to connect to AP.";
	case 0x10 + 0x00C8:
		return "wifi_c_start_sta function timeout expired.";
	case 0x0001 + 0x0258:
		return "NVS namespace was opened in read only mode.";
	case 0x0002 + 0x0258:
		return "NVS was not init.";
	case 0x0003 + 0x0258:
		return "NVS partition was not found.";
	case 0x0004 + 0x0258:
		return "NVS namespace was not found in partition.";
	case 0x0005 + 0x0258:
		return "NVS key was not found in namespace.";
	case 0x0001 + 0x03E8:
		return "Error when trying to connect to remote host.";
	case 0x0002 + 0x03E8:
		return "Device has no internet connection.";
    default:
        #ifdef ESP_PLATFORM
        return esp_err_to_name(err);
        #else
        err = errno;
        return strerror(err);
        #endif
    }
    return "i don't know?";
}
