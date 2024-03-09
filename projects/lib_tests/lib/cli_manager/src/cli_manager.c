/**
 * @file cli_manager.c
 * @author Wojciech Mytych
 * @brief Cli Manager library source file.
 * @version 1.0.9
 * @date 2024-02-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "cli_manager.h"
#include "err_controller.h"
#include "tcp_controller.h"
#include "tcp_c_errors.h"
#include "logger.h"
#include "memory_utils.h"
#include "freertos/event_groups.h"
#include "errno.h"

#include "freertos/task.h"



void cli_accept_task(void* socket) {
    const socket_t listen = (socket_t) socket;
    socket_t client = INVALID_SOCKET;
    err_c_t err = 0;
    while (1)
    {   
        //It should block here until some client shows up
        err = tcp_accept_client(listen, &client);
        if(err == ERR_TCP_INVALID_SOCKET) {
            //listen socket is invalid, from here there is nothing this task can to, delete it.
            LOG_ERROR("error %d, listen socket of cli manager is invalid, deleting cli_accept_task", err);
            vTaskDelete(NULL);
        } else if(err != ERR_C_OK) {
            err = errno;
            LOG_ERROR("error %d when trying to accept new client: %s", err, strerror(err));
            continue;  //next time it may work, just this iteration is broken.
        }

        vTaskDelay(pdMS_TO_TICKS(500));    //delay to setup connection
        logger_add_log_file(fdopen(client, "w+"));
    }
}


int cli_set_remote_logging(uint16_t port, const char* address) {
    volatile err_c_t err = 0;
    socket_t listen = INVALID_SOCKET;
    struct sockaddr_in addr;
    memutil_zero_memory(&addr, sizeof(addr));

    ERR_C_CHECK_NULL_PTR(address, LOG_ERROR("address for cli_manager cannot be NULL"));

    //If address is 0.0.0.0, this means that we have no internet connection, no sense in continuing.
    if(!strncmp("0.0.0.0", address, 8)) {
        err = CLI_ERR_NO_INTERNET;
        LOG_WARN("error %d, device has no internet connection, cannot use remote logging.", err);
        return err;
    }

    Try {
        ERR_C_CHECK_AND_THROW_ERR(tcp_create_socket(&listen));
        ERR_C_CHECK_AND_THROW_ERR(tcp_prepare_address(port, address, &addr));
        ERR_C_CHECK_AND_THROW_ERR(tcp_bind_socket(listen, &addr));
        ERR_C_CHECK_AND_THROW_ERR(tcp_socket_listen(listen, 1));
        LOG_INFO("Connect to device on IP %s with port %u to capture logs.",address, CLI_DEFAULT_PORT);
        xTaskCreate(cli_accept_task, "cli_accept_task", CLI_ACCEPT_TASK_DEFAULT_SIZE, (void*)listen, 2, NULL);
    } Catch(err) {
        LOG_ERROR("Error %d when connecting to remote host: %s", err, error_to_name(err));
        return CLI_ERR_CONNECT_FAIL;
    }
    return 0;
}
