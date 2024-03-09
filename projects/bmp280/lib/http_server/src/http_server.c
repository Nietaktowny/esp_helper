#include "http_server.h"
#include "err_controller.h"
#include "logger.h"
#include "memory_utils.h"
#include "linked_list.h"
#include "linked_list_errors.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_tls_crypto.h"
#include <esp_http_server.h>
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "esp_check.h"


struct http_s_handle_obj {
    httpd_handle_t esp_handle;
    httpd_config_t esp_config;
    linkedl_t handlers;
};

int http_server_init(http_s_handle_t* out_handle) {
    err_c_t err = 0;

    //alloc memory
    NEW((*out_handle), struct http_s_handle_obj);

    err = httpd_start(&((*out_handle)->esp_handle), &((*out_handle)->esp_config));
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d during starting http server: %s", err,  error_to_name(err));
        return err;
    }

    LOG_DEBUG("http server started");
    return err;
}

int http_server_deinit(http_s_handle_t* handle) {
    err_c_t err = 0;

    //dealloc
    DELETE((*handle));

    return err;
}