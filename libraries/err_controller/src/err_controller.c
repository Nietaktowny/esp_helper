#include "err_controller.h"
#include "esp_err.h"


char* err_c_read_esp_err_code(err_c_t err) {
    return esp_err_to_name(err);
}