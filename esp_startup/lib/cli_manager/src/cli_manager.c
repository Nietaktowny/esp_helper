#include "cli_manager.h"
#include "err_controller.h"
#include "errors_list.h"
#include "wifi_controller.h"
#include "tcp_controller.h"
#include "tcp_c_errors.h"
#include "logger.h"
#include "memory_utils.h"


#include "freertos/task.h"

#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"

#define SSID_SOL "OstNet952235plus"
#define PASSWORD_SOL "Solonka106"



int cli_connect_to_wifi(const char* ssid, const char* password) {
    int err = 0;
    CHECK_NULL_PTR(ssid, LOG_ERROR("ssid cannot be null"));
    CHECK_NULL_PTR(password, LOG_ERROR("password cannot be null"));

    err = wifi_c_init_wifi(WIFI_C_MODE_STA);
    if(err != 0) {
        return err;
    }

    err = wifi_c_start_sta(ssid, password);


    return err;
}

void cli_accept_task(void* args) {
    socket_t listen = INVALID_SOCKET;
    socket_t client = INVALID_SOCKET;
    struct sockaddr_in addr;
    memutil_zero_memory(&addr, sizeof(addr));
    tcp_create_socket(&listen);
    tcp_prepare_address(port, address, &addr);
    tcp_bind_socket(listen, &addr);
    tcp_socket_listen(listen, 1);
    while (1)
    {   
        tcp_accept_client(listen, &client);
        vTaskDelay(pdMS_TO_TICKS(1000));
        logger_set_log_output(fdopen(client, "w+"));
    }
}

int set_logging_to_socket(char* address, uint16_t port) {
    socket_t listen = INVALID_SOCKET;
    socket_t client = INVALID_SOCKET;
    struct sockaddr_in addr;
    memutil_zero_memory(&addr, sizeof(addr));

    tcp_create_socket(&listen);
    tcp_prepare_address(port, address, &addr);
    tcp_bind_socket(listen, &addr);
    tcp_socket_listen(listen, 1);
    tcp_accept_client(listen, &client);

    logger_set_log_output(fdopen(client, "w+"));

    return 0;
}