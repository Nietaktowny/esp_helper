#define WIN32_LEAN_AND_MEAN

#include <unistd.h>
#include "tcp_client.h"
#include <unistd.h>
#include "log.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <inttypes.h>



int tcp_client_init_winsock(WSADATA* wsaData) {
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

char* tcp_client_get_server_socket_ip(SOCKET socket) {
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

int tcp_client_get_server_socket_port(SOCKET socket) {
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

int tcp_client_prepare_server_address(struct addrinfo** result, const char* server_addr, const char* port) {
    LOG_DEBUG("Preparing address");
    int err = 0;
    struct addrinfo hints;
 
    memset(&hints, 0, sizeof(struct addrinfo));

    if(!server_addr) {
        LOG_WARN("address cannot be null, pointer %p", server_addr);
        err = ERR_NULL_POINTER;
        return err;
    }

    if(!port) {
        LOG_WARN("port cannot be null, pointer %p", port);
        err = ERR_NULL_POINTER;
        return err;
    }

    if((int)strlen(server_addr) <= 6) {
        LOG_WARN("address to short: %s", server_addr);
        err = WSAHOST_NOT_FOUND;
        return err;
    }

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;


    err = getaddrinfo(server_addr, port, &hints, result);

    if(err != 0) {
        LOG_ERROR("getaddrinfo failed with result %d,\nerror name: %s", err, err_to_name(err));
        err = WSAGetLastError();
        CHECK_ERROR(err);
        CHECK_ERROR(WSACleanup());
    }

    return err;    
}

int tcp_client_set_sockopt_reuse_addr(SOCKET socket) {
    int err = 0;
    BOOL option_value = TRUE;
    int option_len = sizeof(BOOL);

    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&option_value, option_len) != SOCKET_ERROR) {
        LOG_DEBUG("socket can be bound to an address that is already in use");
    } else {
        err = WSAGetLastError();
        LOG_ERROR("error when setting SO_REUSEADDR option: %d,\nerror name: %s", err, err_to_name(err));
        return err;
    }
    return err;
}

int tcp_client_create_connect_socket(SOCKET* connect_socket) {
    int err = 0;

    if(!connect_socket) {
        LOG_WARN("connect_socket cannot be null");
        err = ERR_NULL_POINTER;
        return err;
    }
    LOG_DEBUG("Creating listen socket");
    
    *connect_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    err = tcp_client_set_sockopt_reuse_addr(*connect_socket);

    if(*connect_socket == INVALID_SOCKET) {
        err = WSAGetLastError();
        LOG_FATAL("socket failed with error %d", err);
        WSACleanup();
    }
    return err;    
}

int tcp_client_connect_to_server(SOCKET* connect_socket, struct addrinfo** address) {
    int err = 0;
    int i = 0;
    if(!connect_socket) {
        LOG_WARN("connect_socket cannot be null");
        err = ERR_NULL_POINTER;
        return err;
    }

    if(!address) {
        LOG_WARN("address cannot be null, pointer %p", (void*)address);
        err = ERR_NULL_POINTER;
        return err;
    }

    LOG_DEBUG("attempting connecting to server...");

    // Attempt to connect to an address until one succeeds
    for(struct addrinfo* ptr = *address; ptr != NULL; ptr = ptr->ai_next) {
        LOG_DEBUG("trying to connect %d time", i++);
        err = tcp_client_create_connect_socket(connect_socket);

        if(err != 0) {
            LOG_ERROR("failed to create socket when connecting: %d\nerror name: %s", err, err_to_name(err));
            return err;
        }

        err = connect(*connect_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if(err == SOCKET_ERROR) {
            closesocket(*connect_socket);
            *connect_socket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    if(*connect_socket == INVALID_SOCKET) {
        LOG_ERROR("unable to connect to server");
        err = WSAGetLastError();
        WSACleanup();
        return err;
    }

    LOG_INFO("connected to server: %s:%d", tcp_client_get_server_socket_ip(*connect_socket), tcp_client_get_server_socket_port(*connect_socket));

    LOG_DEBUG("freeing resources");
    freeaddrinfo(*address);

    return err;
}

void* tcp_client_handler_thread(void* args) {
    int err = 0;
    char sendbuf[256] = "init data";
    SOCKET connect_socket = (SOCKET) args;

    if(connect_socket == INVALID_SOCKET) {
        LOG_ERROR("socket passed to handler thread is invalid");
        pthread_exit(NULL);
    }
    LOG_DEBUG("handling socket transmission: %lld", connect_socket);

    while (1) {
        sleep(3);
        //SEND
        err = send(connect_socket, &sendbuf[0], sizeof(sendbuf), 0);

        if(err == SOCKET_ERROR) {
            err = WSAGetLastError();
            LOG_ERROR("send failed with error: %d\nerror name: %s", err, err_to_name(err));
            closesocket(connect_socket);
            WSACleanup();
            pthread_exit(NULL);
        }

        LOG_DEBUG("bytes sent: %d", err);

        err = 0;

        //RECEIVE
        do {
            err = recv(connect_socket, &sendbuf[0], sizeof(sendbuf), 0);

            if(err > 0) {
                LOG_DEBUG("bytes received: %d", err);
            } else if (err == 0) {
                LOG_INFO("connection %lld closed", connect_socket);
            } else {
                err = WSAGetLastError();
                LOG_ERROR("recv failed with error: %d\nerror name: %s",err, err_to_name(err));
                break;
            }
        } while (err > 0);
    }
    return NULL;
}

int tcp_client_start_handler_thread(SOCKET* connect_socket, pthread_t* handler_thread) {
    int err = 0;
    LOG_DEBUG("starting handler thread");
    err = pthread_create(handler_thread, NULL, tcp_client_handler_thread, (void*) (*connect_socket));

    if(err != 0) {
        LOG_ERROR("error when creating handler thread, error code: %d", errno);
        err = errno;
    }
    err = pthread_join(*handler_thread, NULL);
    return err;
}

int tcp_client_convert_data_to_network_order(tcp_client_data_t* data) {
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

int tcp_client_convert_data_to_host_order(tcp_client_data_t* data) {
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

int tcp_client_send_data(tcp_client_handle_t* client_handle, tcp_client_data_t* data) {
    int err = 0;
    SOCKET connect_socket = client_handle->connect_socket;

    //before sending, convert to network order
    tcp_client_convert_data_to_network_order(data);


    err = send(connect_socket, (char*) data, sizeof(tcp_client_data_t), 0);

    if(err == SOCKET_ERROR) {
        err = WSAGetLastError();
        LOG_ERROR("send failed with error: %d\nerror name: %s", err, err_to_name(err));
        closesocket(connect_socket);
        WSACleanup();
        return err;
    }

    LOG_DEBUG("bytes sent: %d", err);
    return 0;
}

int tcp_client_recv_data(tcp_client_handle_t* client_handle, tcp_client_data_t* data) {
    int err = 0;
    SOCKET connect_socket = client_handle->connect_socket;
    err = recv(connect_socket, (char*) data, sizeof(tcp_client_data_t), MSG_WAITALL);

    //after receiving convert data from network to host order
    tcp_client_convert_data_to_host_order(data);

    if(err > 0) {
        LOG_DEBUG("bytes received: %d", err);
        LOG_DEBUG("data len: %u", data->data_len);
        LOG_DEBUG("data sync: %u", data->sync);
        LOG_DEBUG("data: %s", data->data);
        LOG_DEBUG("cmd base: %d", data->cmd_base);
        LOG_DEBUG("cmd: %d", data->cmd);
    } else if (err == 0) {
        LOG_WARN("connection %lld closed", connect_socket);
        return WSAECONNRESET;
    } else {
        err = WSAGetLastError();
        if(err == WSAECONNRESET) {
            LOG_WARN("connection %lld closed", connect_socket);
            return err;
        } else {
            LOG_ERROR("recv failed with error: %d\nerror name: %s",err, err_to_name(err));
        }
        return err;
    }
    return 0;
}

int tcp_client_close_connection(tcp_client_handle_t* client_handle) {
    int err = 0;

    if(!client_handle) {
        err = ERR_NULL_POINTER;
        LOG_ERROR("client handle cannot be null");
        return err;
    }
    LOG_DEBUG("closing socket %lld", client_handle->connect_socket);

    err = closesocket(client_handle->connect_socket);

    if(err == SOCKET_ERROR) {
        err = WSAGetLastError();
        LOG_ERROR("error when closing socket: %d\nerror name:%s", err, err_to_name(err));
        return err;
    }

    LOG_DEBUG("socket closed");

    return err;
}

int tcp_client_init(tcp_client_handle_t** const client_handle, const char* server_ip, const char* server_port) {
    int err = 0;

    if(!client_handle) {
        err = ERR_NULL_POINTER;
        LOG_ERROR("client handle cannot be null");
        return err;
    }


    *client_handle = malloc(sizeof(tcp_client_handle_t));

    if(*client_handle == NULL) {
        LOG_FATAL("error when allocating memory for tcp_client");
        err = ERR_NO_MEMORY;
        return err;
    }

    err = tcp_client_init_winsock(&((*client_handle)->wsaData));
    if(err != 0) {
        LOG_ERROR("error when initializing winsock: %d,\nerror name: %s", err, err_to_name(err));
        return err;
    }

    err = tcp_client_prepare_server_address(&((*client_handle)->result), server_ip, server_port);
    if(err != 0) {
        LOG_ERROR("error when preparing server address: %d,\nerror name: %s", err, err_to_name(err));
        return err;
    }

    err = tcp_client_connect_to_server(&((*client_handle)->connect_socket), &((*client_handle)->result));
    if(err != 0) {
        LOG_ERROR("error when trying to connect: %d,\nerror name: %s", err, err_to_name(err));
        return err;
    }

    /*
    err = tcp_client_start_handler_thread(&((*client_handle)->connect_socket), &((*client_handle)->handler_thread));
    if(err != 0) {
        LOG_ERROR("error when starting handler thread: %d,\nerror name: %s", err, err_to_name(err));
        return err;
    }
    */
    return err;
}