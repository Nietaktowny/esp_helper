#include "cli_manager.h"
#include "err_controller.h"
#include "errors_list.h"
#include "tcp_controller.h"
#include "tcp_c_errors.h"
#include "logger.h"
#include "memory_utils.h"
#include "freertos/event_groups.h"


#include "freertos/task.h"



void cli_accept_task(void* args) {
    socket_t listen = (socket_t) args;
    socket_t client = INVALID_SOCKET;
    while (1)
    {   
        //It should block here until some client shows up
        tcp_accept_client(listen, &client);
        vTaskDelay(pdMS_TO_TICKS(1000));    //delay to setup connection
        logger_add_log_file(fdopen(client, "w+"));
    }
}

/**
 * @todo If address is 0.0.0.0 don't bind, no internet connection.
 */
int cli_set_remote_logging(uint16_t port, const char* address) {
    volatile err_c_t err = 0;
    socket_t listen = INVALID_SOCKET;
    socket_t client = INVALID_SOCKET;
    struct sockaddr_in addr;
    memutil_zero_memory(&addr, sizeof(addr));

    ERR_C_CHECK_NULL_PTR(address, LOG_ERROR("address for cli_manager cannot be NULL"));

    //If address is 0.0.0.0, this means that we have no internet connection, no sense in continuing.
    if(!strncmp("0.0.0.0", address, 8)) {
        LOG_WARN("Device has no internet connection, cannot use remote logging.");
        return CLI_ERR_NO_INTERNET;
    }

    Try {
        ERR_C_CHECK_AND_THROW_ERR(tcp_create_socket(&listen));
        ERR_C_CHECK_AND_THROW_ERR(tcp_prepare_address(port, address, &addr));
        ERR_C_CHECK_AND_THROW_ERR(tcp_bind_socket(listen, &addr));
        ERR_C_CHECK_AND_THROW_ERR(tcp_socket_listen(listen, 1));
        LOG_INFO("Connect to device on IP %s with port %u to capture logs.",address, 27015);
        ERR_C_CHECK_AND_THROW_ERR(tcp_accept_client(listen, &client));
        vTaskDelay(pdMS_TO_TICKS(1000));    //delay to setup connection
        logger_add_log_file(fdopen(client, "w+"));
        xTaskCreate(cli_accept_task, "accept_task", 2048, (void*)listen, 2, NULL);
    } Catch(err) {
        LOG_ERROR("Error %d when connecting to remote host: %s", err, error_to_name(err));
        return CLI_ERR_CONNECT_FAIL;
    }
    return 0;
}