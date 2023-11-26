#include "cli_manager.h"
#include "err_controller.h"
#include "errors_list.h"
#include "wifi_controller.h"
#include "tcp_controller.h"
#include "tcp_c_errors.h"
#include "logger.h"
#include "memory_utils.h"
#include "freertos/event_groups.h"


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
    socket_t listen = (socket_t) args;
    socket_t client = INVALID_SOCKET;
    while (1)
    {   
        //It should block here until some client shows up
        tcp_accept_client(listen, &client);
        vTaskDelay(pdMS_TO_TICKS(1000));    //delay to setup connection
        logger_set_log_output(fdopen(client, "w+"));
    }
}

int cli_set_remote_logging(void) {
    socket_t listen = INVALID_SOCKET;
    socket_t client = INVALID_SOCKET;
    struct sockaddr_in addr;
    memutil_zero_memory(&addr, sizeof(addr));
    tcp_create_socket(&listen);
    tcp_prepare_address(27015, wifi_c_get_ipv4(), &addr);
    tcp_bind_socket(listen, &addr);
    tcp_socket_listen(listen, 1);
    LOG_INFO("Connect to device on IP %s with port %u to capture logs.", wifi_c_get_ipv4(), 27015);
    tcp_accept_client(listen, &client);
    vTaskDelay(pdMS_TO_TICKS(1000));    //delay to setup connection
    logger_set_log_output(fdopen(client, "w+"));
    xTaskCreate(cli_accept_task, "accept_task", 2048, (void*)listen, 2, NULL);
    return 0;
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