#include <stddef.h>
#include <inttypes.h>

#define OTA_C_ERR_BASE                      0x0230
#define OTA_C_ERR_IMAGES_THE_SAME           OTA_C_ERR_BASE + 0x0001

int ota_c_do_simple_ota(const char* url);

int ota_c_update_device_data(const char* url, const char* path, const char* device_id, const char* board);

int ota_c_prepare_url_with_device_id(char* url, uint64_t device_id, char* buffer, size_t buflen);

int ota_c_start(const char* url);