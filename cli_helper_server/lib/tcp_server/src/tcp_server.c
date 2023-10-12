 #define WIN32_LEAN_AND_MEAN

#include <unistd.h>
#include "tcp_server.h"
#include "tcp_server_internal.h"
#include <unistd.h>
#include "log.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887" 


int tcp_server_init_winsock(WSADATA* wsaData) {
    LOG_DEBUG("initializing Winsock");
    
    int err = WSAStartup(MAKEWORD(2, 2), wsaData);

    if (err != 0) {
        LOG_ERROR("Winsock initialization failed with result %d", err);
        err = WSAGetLastError();
        CHECK_ERROR(err);
    }

    LOG_DEBUG("Winsock initialized");
    return err;
}

int tcp_server_prepare_address(struct addrinfo** result, const char* address) {
    LOG_DEBUG("Preparing address");
    int err = 0;
    struct addrinfo hints;
 
    memset(&hints, 0, sizeof(struct addrinfo));

    if(!address) {
        LOG_WARN("address cannot be null, pointer %p", address);
        err = ERR_NULL_POINTER;
        return err;
    }

    if((int)strlen(address) <= 6) {
        LOG_WARN("address to short: %s", address);
        err = WSAHOST_NOT_FOUND;
        return err;
    }

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    err = getaddrinfo(address, DEFAULT_PORT, &hints, result);

    if(err != 0) {
        LOG_ERROR("getaddrinfo failed with result %d,\nerror name: %s", err, err_to_name(err));
        err = WSAGetLastError();
        CHECK_ERROR(err);
        CHECK_ERROR(WSACleanup());
    }

    return err;
}

int tcp_server_create_listen_socket(SOCKET* listen_socket) {
    int err = 0;

    if(!listen_socket) {
        LOG_WARN("connect_socket cannot be null, pointer");
        err = ERR_NULL_POINTER;
        return err;
    }

    LOG_DEBUG("Creating listen socket");
    
    *listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    err = tcp_server_set_sockopt_reuse_addr(*listen_socket);

    if(*listen_socket == INVALID_SOCKET) {
        err = WSAGetLastError();
        LOG_FATAL("socket failed with error %d", err);
        WSACleanup();
    }

    return err;
}

int tcp_server_bind_socket(SOCKET* socket, struct addrinfo** result) {
    int err = 0;
    LOG_DEBUG("binding socket");
    
    err = bind(*socket, (*result)->ai_addr, (int)((*result)->ai_addrlen));

    if(err == (int)INVALID_SOCKET) {
        err = WSAGetLastError();
        LOG_ERROR("bind failed with error %d,\nerror name: %s", err, err_to_name(err));
        freeaddrinfo(*result);
        closesocket(*socket);
        WSACleanup();
    }
    LOG_INFO("bound socket to address: %s:%d", tcp_server_get_server_socket_ip(*socket), tcp_server_get_server_socket_port(*socket));

    return err;
}

int tcp_server_listen_on_socket(SOCKET* listen_socket) {
    int err = 0;
    LOG_DEBUG("listening on socket");

    if(*listen_socket == INVALID_SOCKET) {
        LOG_ERROR("Socket cannot be invalid");
        return WSAENOTSOCK;
    }

    err = listen(*listen_socket, TCP_BACKLOG);

    if(err == SOCKET_ERROR) {
        err = WSAGetLastError();
        LOG_ERROR("listen failed with error: %d", err);
        LOG_ERROR("Error name: %s", err_to_name(err));
        closesocket(*listen_socket);
        WSACleanup();
        return -1;
    }

    return 0;
}

SOCKET tcp_server_accept_client(tcp_server_handle_t* server_handle) {
    int err = 0;
    LOG_DEBUG("waiting for accept on listen socket");

    if(server_handle->listen_socket == INVALID_SOCKET) {
        LOG_ERROR("Socket cannot be invalid");
        return WSAENOTSOCK;
    }

    SOCKET client_socket = accept(server_handle->listen_socket, NULL, NULL);

    if(client_socket == INVALID_SOCKET) {
        err = WSAGetLastError();
        LOG_ERROR("accept failed with error: %d", err);
        LOG_ERROR("Error name: %s", err_to_name(err));
        closesocket(server_handle->listen_socket);
        WSACleanup();
        return 1;
    }

    LOG_DEBUG("new connection accepted");
    LOG_DEBUG("connected to socket: %s:%d", tcp_server_get_client_socket_ip(client_socket), tcp_server_get_client_socket_port(client_socket));
    return client_socket;
}

int tcp_server_toggle_socket_block_mode(SOCKET socket, int mode_val) {
    int err = 0;
    long unsigned int mode = mode_val;

    err = ioctlsocket(socket, FIONBIO, &mode);

    if(err != 0) {
        LOG_ERROR("ioctlsocket failed with error: %d", err);
    }

    return err;
}

int tcp_server_set_sockopt_reuse_addr(SOCKET socket) {
    int err = 0;
    BOOL option_value = TRUE;
    int option_len = sizeof(BOOL);

    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&option_value, option_len) != SOCKET_ERROR) {
        LOG_DEBUG("socket can be bound to an address that is already in use.");
    } else {
        err = WSAGetLastError();
        LOG_ERROR("error when setting SO_REUSEADDR option: %d,\nerror name: %s", err, err_to_name(err));
        return err;
    }
    return err;
}

char* tcp_server_get_server_socket_ip(SOCKET socket) {
    int err = 0;
    struct sockaddr_in socket_addr;
    int namelen = sizeof(socket_addr);
    
    memset(&socket_addr, 0, sizeof(socket_addr));

    err = getsockname(socket, (struct sockaddr*) &socket_addr, &namelen);

    if(err != 0) {
        err = WSAGetLastError();
        LOG_ERROR("error when getting server socket name %d,\nerror name: %s", err, err_to_name(err));
    }

    return inet_ntoa(socket_addr.sin_addr);
}

int tcp_server_get_server_socket_port(SOCKET socket) {
    int err = 0;
    struct sockaddr_in socket_addr;
    int namelen = sizeof(socket_addr);
    
    memset(&socket_addr, 0, sizeof(socket_addr));

    err = getsockname(socket, (struct sockaddr*) &socket_addr, &namelen);

    if(err != 0) {
        err = WSAGetLastError();
        LOG_ERROR("error when getting server socket name %d,\nerror name: %s", err, err_to_name(err));
        return -1;
    }

    return (int)ntohs(socket_addr.sin_port);
}

char* tcp_server_get_client_socket_ip(SOCKET socket) {
    int err = 0;
    struct sockaddr_in socket_addr;
    int namelen = sizeof(socket_addr);
    
    memset(&socket_addr, 0, sizeof(socket_addr));

    err = getsockname(socket, (struct sockaddr*) &socket_addr, &namelen);

    if(err != 0) {
        err = WSAGetLastError();
        LOG_ERROR("error when getting client socket name %d,\nerror name: %s", err, err_to_name(err));
    }

    return inet_ntoa(socket_addr.sin_addr);
}

int tcp_server_get_client_socket_port(SOCKET socket) {
    int err = 0;
    struct sockaddr_in socket_addr;
    int namelen = sizeof(socket_addr);
    
    memset(&socket_addr, 0, sizeof(socket_addr));

    err = getpeername(socket,(struct sockaddr*) &socket_addr, &namelen);

    if(err != 0) {
        err = WSAGetLastError();
        LOG_ERROR("error when getting client socket name %d,\nerror name: %s", err, err_to_name(err));
        return -1;
    }

    return (int)ntohs(socket_addr.sin_port);
}

int tcp_server_convert_data_to_network_order(tcp_server_data_t* data) {
    int err = 0;
    if(!data) {
        LOG_ERROR("data cannot be null");
        err = ERR_NULL_POINTER;
        return err;
    }

    // Only sync needs to be changed because it's only 2 bytes value in data
    data->sync = htons(data->sync);
    
    return 0;
}

int tcp_server_convert_data_to_host_order(tcp_server_data_t* data) {
    int err = 0;
    if(!data) {
        LOG_ERROR("data cannot be null");
        err = ERR_NULL_POINTER;
        return err;
    }

    // Only sync needs to be changed because it's only 2 bytes value in data
    data->sync = ntohs(data->sync);
    
    return 0;
}

int tcp_server_recv_data(SOCKET client_socket, tcp_server_data_t* data) {
    int err = 0;
    err = recv(client_socket, (char*) data, sizeof(tcp_server_data_t), MSG_WAITALL);

    if(err > 0) {
        LOG_DEBUG("bytes received: %d", err);
        LOG_DEBUG("data len: %u", data->data_len);
        LOG_DEBUG("data sync: %u", data->sync);
        LOG_DEBUG("data: %s", data->data);
        LOG_DEBUG("cmd base: %d", data->cmd_base);
        LOG_DEBUG("cmd: %d", data->cmd);
    } else if (err == 0) {
        LOG_WARN("connection %lld closed", client_socket);
        return WSAECONNRESET;
    } else {
        err = WSAGetLastError();
        if(err == WSAECONNRESET) {
            LOG_WARN("connection %lld closed", client_socket);
            return err;
        } else {
            LOG_ERROR("recv failed with error: %d\nerror name: %s",err, err_to_name(err));
        }
        return err;
    }
    return 0;
}

void* tcp_server_handler_thread(void* args) {
    int err = 0;
    tcp_server_data_t data;
    SOCKET client_socket = (SOCKET) args;
    LOG_DEBUG("handling socket transmission: %lld\n", client_socket);
    memset(&data, 0, sizeof(data));
    while (1) {
        //RECEIVE
        //tcp_server_toggle_socket_block_mode(client_socket, 1);
        err = recv(client_socket, (char*)&data, sizeof(tcp_server_data_t), MSG_WAITALL);

        //after receiving convert data from network to host order
        if(tcp_server_convert_data_to_host_order(&data) != 0) {
            LOG_WARN("error when converting data to host order: %d\nerror name: %s", err, err_to_name(err));
        }

        if(err > 0) {
            LOG_DEBUG("bytes received: %d", err);
            LOG_DEBUG("data len: %u", data.data_len);
            LOG_DEBUG("data sync: %u", data.sync);
            LOG_DEBUG("data: %s", data.data);
        } else if (err == 0) {
            LOG_DEBUG("connection %lld closed", client_socket);
            break;
        } else {
            err = WSAGetLastError();

            if(err == WSAECONNRESET) {
                LOG_WARN("%s", err_to_name(err));
                break;
            }
            LOG_ERROR("recv failed with error: %d\nerror name: %s",err, err_to_name(err));
            closesocket(client_socket);
            WSACleanup();
            pthread_exit(NULL);
        }


        //SEND
        char* server_msg = "data from server";
        memset(&(data.data[0]), 0, sizeof(data.data));
        strncpy(&(data.data[0]), server_msg, strlen(server_msg));
        data.data_len = strlen("data from server");

        //before sending, convert to network order
        if(tcp_server_convert_data_to_network_order(&data) != 0) {
            LOG_WARN("error when converting data to network order: %d\nerror name: %s", err, err_to_name(err));
        }
        err = send(client_socket, (char*) &data, sizeof(tcp_server_data_t), 0);

        if(err == SOCKET_ERROR) {
            err = WSAGetLastError();
            if(err == WSAECONNRESET) {
                LOG_WARN("%s", err_to_name(err));
                break;
            }
            LOG_ERROR("send failed with error: %d\nerror name: %s", err, err_to_name(err));
            closesocket(client_socket);
            WSACleanup();
            pthread_exit(NULL);
        }

        LOG_DEBUG("bytes sent: %d", err);

        err = 0;
    }
    LOG_DEBUG("finished transmission");
    closesocket(client_socket);
    LOG_DEBUG("closed socket: %lld", client_socket);
    return NULL;
}

int tcp_server_start_handler_thread(SOCKET client_socket, tcp_server_handle_t* server_handle) {
    int err = 0;
    LOG_DEBUG("starting handler thread");
    err = pthread_create(&(server_handle->handler_thread), NULL, tcp_server_handler_thread, (void*) client_socket);

    if(err != 0) {
        LOG_ERROR("error when creating handler thread, error code: %d", errno);
        err = errno;
    }

    return err;
}

void* tcp_server_listen_thread(void* args) {
    tcp_server_handle_t* server_handle = (tcp_server_handle_t*) args;
    while(1) {
        LOG_DEBUG("waiting for connection to accept");
        SOCKET new_client_socket = tcp_server_accept_client(server_handle);
        LOG_INFO("accepted new connection: %s:%d", tcp_server_get_client_socket_ip(new_client_socket), tcp_server_get_client_socket_port(new_client_socket));
        tcp_server_start_handler_thread(new_client_socket, server_handle);
    }

    return NULL;
}

int tcp_server_start_listen_thread(tcp_server_handle_t* server_handle) {
    int err = 0;

    LOG_INFO("listening for incoming connections...");
    err = pthread_create(&(server_handle->listen_thread), NULL, tcp_server_listen_thread, (void*) server_handle);

    if(err != 0) {
        LOG_ERROR("error when creating listen thread, error code: %d", errno);
        err = errno;
    }
    err = pthread_join(server_handle->listen_thread, NULL);
    return err;
}

int tcp_server_init(tcp_server_handle_t** server_handle, int listen_port, const char* ip_address) {
    int err = 0;

    LOG_DEBUG("allocating memory for tcp_server...");
    *server_handle = (tcp_server_handle_t*) malloc(sizeof(tcp_server_handle_t));

    if(!(*server_handle)) {
        err = ERR_NO_MEMORY;
        LOG_FATAL("no memory for tcp server");
        return err;
    }

    (*server_handle)->port = listen_port;

    err = tcp_server_init_winsock(&((*server_handle)->wsaData));

    if(err != 0) {
        return err;
    }

    err = tcp_server_prepare_address(&(*server_handle)->result, ip_address);

    if(err != 0) {
        return err;
    }

    err = tcp_server_create_listen_socket(&((*server_handle)->listen_socket));
    if(err != 0) {
        return err;;
    }

    err = tcp_server_bind_socket(&((*server_handle)->listen_socket), &(*server_handle)->result);

    if(err != 0) {
        return err;
    }

    err = tcp_server_listen_on_socket(&((*server_handle)->listen_socket));

    if(err != 0) {
        return err;
    }

    return err;
}

int tcp_server_deinit(tcp_server_handle_t** server_handle) {
    int err = 0;

    err = shutdown((*server_handle)->listen_socket, SD_BOTH);
    if(err != 0) {
        err= WSAGetLastError();
        if(err != WSAENOTCONN) {
            LOG_ERROR("error when trying to shutdown the socket: %d,\nerror name: %s", err, err_to_name(err));
            return err;
        } else {
            LOG_DEBUG("trying to shutdown not connected socket");
        }
    }

    err = closesocket((*server_handle)->listen_socket);
    if(err != 0 && err != WSAENOTCONN) {
        err = WSAGetLastError();
        if(err != WSAENOTCONN) {
            LOG_ERROR("error when closing socket: %d,\nerror name: %s", err, err_to_name(err));
            return err;
        } else {
            LOG_DEBUG("trying to close not connected socket");
        }
    }

    if((*server_handle)->handler_thread) {
        err = pthread_cancel((*server_handle)->handler_thread);
    }

    if(err != 0 ) {
        LOG_ERROR("error when stopping handler thread: %d", err);
        return err;
    }

    if((*server_handle)->listen_thread) {
        err = pthread_cancel((*server_handle)->listen_thread);
    }

    if(err != 0 ) {
        LOG_ERROR("error when stopping listen thread: %d", err);
        return err;
    }

    err = WSACleanup();

    if(err != 0) {
        err = WSAGetLastError();
        LOG_ERROR("error when deinitializing winsock: %d", err);
        return err;
    }


    memset(*server_handle, 0, sizeof(tcp_server_handle_t));
    free(*server_handle);
    *server_handle = NULL;
    return err;
}