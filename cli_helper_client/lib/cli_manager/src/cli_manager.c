#include "cli_manager.h"
#include "tcp_driver.h"
#include "tcp_driver_errors.h"
#include "memory_utils.h"
#include "logger.h"
#include "CException.h"
#include "err_controller.h"
#include "errors_list.h"




int cli_create_logging_server(char* address, uint16_t port) {
    volatile err_c_t err = 0;
    socket_t listen = INVALID_SOCKET;
    socket_t client = INVALID_SOCKET;
    struct sockaddr_in addr;
    memutil_zero_memory(&addr, sizeof(addr));

    Try {
        ERR_C_CHECK_AND_THROW_ERR(tcp_create_socket(&listen));
        ERR_C_CHECK_AND_THROW_ERR(tcp_prepare_address(port, address, &addr));
        ERR_C_CHECK_AND_THROW_ERR(tcp_set_reuse_addr(listen));
        ERR_C_CHECK_AND_THROW_ERR(tcp_bind_socket(listen, &addr));
        ERR_C_CHECK_AND_THROW_ERR(tcp_socket_listen(listen, 1));
        ERR_C_CHECK_AND_THROW_ERR(tcp_accept_client(listen, &client));

        logger_set_log_output(fdopen(client, "w+"));
    } Catch(err) {
        LOG_ERROR("error %u when setting logging server: %s", err, error_to_name(err));
    }
    LOG_DEBUG("cli manager successfully prepared to redirect logs to socket");
    return ERR_C_OK;
}


int cli_listen_for_logs(char* address, uint16_t port, uint8_t log_level) {
    volatile err_c_t err = 0;
    socket_t server = INVALID_SOCKET;
    struct sockaddr_in addr;
    memutil_zero_memory(&addr, sizeof(addr));

    Try {
        ERR_C_CHECK_AND_THROW_ERR(tcp_create_socket(&server));
        ERR_C_CHECK_AND_THROW_ERR(tcp_prepare_address(port, address, &addr));
        ERR_C_CHECK_AND_THROW_ERR(tcp_connect_ipv4(server, (struct sockaddr*)&addr));
    } Catch(err) {
        LOG_ERROR("error %u when trying to listen for logs: %s", err, err_c_to_name(err));
    }
    logger_set_log_level(log_level);

    while (1)
    {
        char data[255];
        memutil_zero_memory(&data, sizeof(data));
        tcp_recv(server, &data, sizeof(data));
        LOG_INFO("received: %s", data);
    }
    
    return ERR_C_OK;
}