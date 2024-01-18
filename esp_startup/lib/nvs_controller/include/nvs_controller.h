#include <stddef.h>

typedef struct nvs_c_handle_obj* nvs_c_handle_t;

#define NVS_C_ERR_BASE                  0x7040
#define NVS_C_ERR_WRONG_MODE            NVS_C_ERR_BASE + 0x0001


int nvs_c_init_nvs (void);

int nvs_c_open(nvs_c_handle_t* handle, char* namespace);

int nvs_c_open_read_only(nvs_c_handle_t* handle, char* namespace);

int nvs_c_close(nvs_c_handle_t handle);

int nvs_c_write_string(nvs_c_handle_t handle, const char* key, const char* buffer);

int nvs_c_read_string(nvs_c_handle_t handle, const char* key, char* buffer, size_t buflen);