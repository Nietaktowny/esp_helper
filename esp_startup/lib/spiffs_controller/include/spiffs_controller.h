#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

struct spiffs_c_handle_obj {
    char* base_name;
    char* partition_label;
    uint8_t max_files;
    bool format_on_err;
};

typedef struct spiffs_c_handle_obj spiffs_c_handle_t;


int spiffs_c_init(const spiffs_c_handle_t* handle);

int spiffs_c_check(const char* partition_label);

int spiffs_c_format(const char* partition);

int spiffs_c_get_info(const char* partition_label, size_t* out_total, size_t* out_used);

int spiffs_c_open_file(spiffs_c_handle_t* handle, char* filename, const char* mode, FILE** out_file);

void spiffs_c_close_file(FILE* file);