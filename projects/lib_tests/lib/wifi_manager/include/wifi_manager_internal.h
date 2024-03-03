


#include <stddef.h>

struct wifi_s_handle_obj;
typedef struct wifi_s_handle_obj* wifi_s_handle_t;

int wifi_manager_server_init(wifi_s_handle_t* out_handle);

int wifi_manager_get_scanned_aps(char* out_buffer, size_t buflen);

int wifi_manager_get_stored_ap_as_json(char* buffer, size_t bufflen);