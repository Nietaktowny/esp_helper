#include "tcp_client.h"
#include <unistd.h>
#include "log.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "sys/socket.h"
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/param.h> //for htonll


static uint64_t htonll(uint64_t n);
static uint64_t ntohll(uint64_t n);


static uint64_t htonll(uint64_t n) {
    #if __BYTE_ORDER == __BIG_ENDIAN
        return n; 
    #else
        return (((uint64_t)htonl(n)) << 32) + htonl(n >> 32);
    #endif
}

static uint64_t ntohll(uint64_t n) {
    #if __BYTE_ORDER == __BIG_ENDIAN
        return n;
    #else
        return (((uint64_t)ntohl(n)) << 32) + ntohl(n >> 32);
    #endif
}

char* tcp_client_get_server_socket_ip(Socket_t socket) {
    int err = 0;
    struct sockaddr_in socket_addr;
    socklen_t namelen = sizeof(socket_addr);
    
    memset(&socket_addr, 0, sizeof(socket_addr));

    err = getsockname(socket, (struct sockaddr*) &socket_addr, &namelen);

    if(err != 0) {
        err = errno;
        LOG_ERROR("error when getting client socket name %d,\nerror name: %s", err, err_to_name(err));
    }

    return inet_ntoa(socket_addr.sin_addr);
}

int tcp_client_get_server_socket_port(Socket_t socket) {
    int err = 0;
    struct sockaddr_in socket_addr;
    socklen_t namelen = sizeof(socket_addr);
    
    memset(&socket_addr, 0, sizeof(socket_addr));

    err = getpeername(socket,(struct sockaddr*) &socket_addr, &namelen);

    if(err != 0) {
        err = errno;
        LOG_ERROR("error when getting client socket name %d,\nerror name: %s", err, err_to_name(err));
        return -1;
    }

    return (int)ntohs(socket_addr.sin_port);
}

int tcp_client_prepare_server_address(struct sockaddr_in** result, const char* server_addr, uint16_t port) {
    LOG_DEBUG("preparing address");
    int err = 0;
 
    if(!server_addr) {
        LOG_WARN("address cannot be null, pointer %p", server_addr);
        err = ERR_NULL_POINTER;
        return err;
    }

    if(!port) {
        LOG_WARN("port cannot be 0, port %u", port);
        err = INVALID_SOCKET;
        return err;
    }

    if(!result) {
        LOG_ERROR("address structure cannot be null, pointer %p", (void*) result);
        err = ERR_NULL_POINTER;
        return err;
    }

    // allocate resources for sockaddr_in structure
    (*result) = (struct sockaddr_in*) malloc(sizeof(struct sockaddr_in));
    (*result)->sin_family = AF_INET;
    (*result)->sin_port = htons(port);
    err = inet_aton(server_addr, &((*result)->sin_addr));

    if(err != 1) {
        LOG_ERROR("error when translating address to binary data");
        return INVALID_SOCKET;
    }

    err = 0; //inet_aton returns 1 upon success, change it back to 0

    return err;    
}

int tcp_client_set_sockopt_reuse_addr(Socket_t socket) {
    int err = 0;
    int option_value = true;

    /*
     * turn off bind address checking, and allow
     * port numbers to be reused - otherwise
     * the TIME_WAIT phenomenon will prevent
     * binding to these address.port combinations
     * for (2 * MSL) seconds.
     */

    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&option_value, sizeof(option_value)) != INVALID_SOCKET) {
        LOG_DEBUG("socket can be bound to an address that is already in use.");
    } else {
        err = errno;
        LOG_ERROR("error when setting SO_REUSEADDR option: %d,\nerror name: %s", err, err_to_name(err));
        return err;
    }
    return err;
}

int tcp_client_create_connect_socket(Socket_t* connect_socket) {
    int err = 0;

    if(!connect_socket) {
        LOG_WARN("connect_socket cannot be null");
        err = ERR_NULL_POINTER;
        return err;
    }
    LOG_DEBUG("creating connect socket");
    
    *connect_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    err = tcp_client_set_sockopt_reuse_addr(*connect_socket);

    if(*connect_socket == INVALID_SOCKET) {
        err = errno;
        LOG_FATAL("socket failed with error %d", err);
    }
    LOG_DEBUG("socket successfully created:%d", *connect_socket);
    return err;    
}

int tcp_client_connect_to_server(Socket_t* connect_socket, struct sockaddr_in** result) {
    int err = 0;
    if(!connect_socket) {
        LOG_WARN("connect_socket cannot be null");
        err = ERR_NULL_POINTER;
        return err;
    }

    if(!result || !(*result)) {
        LOG_WARN("address cannot be null, pointer %p", (void*)result);
        err = ERR_NULL_POINTER;
        return err;
    }

    LOG_DEBUG("attempting connecting to server...");
    err = tcp_client_create_connect_socket(connect_socket);
    if(err != 0) {
        LOG_ERROR("failed to create socket when connecting: %d\nerror name: %s", err, err_to_name(err));
        return err;
    }
    err = connect(*connect_socket, (struct sockaddr*) (*result), sizeof(struct sockaddr));
    if(err == INVALID_SOCKET) {
        close(*connect_socket);
        *connect_socket = INVALID_SOCKET;
    }

    if(*connect_socket == INVALID_SOCKET) {
        LOG_ERROR("unable to connect to server");
        err = errno;
        return err;
    }

    LOG_INFO("connected to server: %s:%d", tcp_client_get_server_socket_ip(*connect_socket), tcp_client_get_server_socket_port(*connect_socket));

    return err;
}

void* tcp_client_handler_thread(void* args) {
    int err = 0;
    char sendbuf[256] = "init data";
    Socket_t* connect_socket = (Socket_t*) args;

    if(*connect_socket == INVALID_SOCKET) {
        LOG_ERROR("socket passed to handler thread is invalid");
        pthread_exit(NULL);
    }
    LOG_DEBUG("handling socket transmission: %d", *connect_socket);

    while (1) {
        sleep(3);
        //SEND
        err = send(*connect_socket, &sendbuf[0], sizeof(sendbuf), 0);

        if(err == INVALID_SOCKET) {
            err = errno;
            LOG_ERROR("send failed with error: %d\nerror name: %s", err, err_to_name(err));
            close(*connect_socket);
            pthread_exit(NULL);
        }

        LOG_DEBUG("bytes sent: %d", err);

        err = 0;

        //RECEIVE
        do {
            err = recv(*connect_socket, &sendbuf[0], sizeof(sendbuf), 0);

            if(err > 0) {
                LOG_DEBUG("bytes received: %d", err);
            } else if (err == 0) {
                LOG_INFO("connection %d closed", *connect_socket);
            } else {
                err = errno;
                LOG_ERROR("recv failed with error: %d\nerror name: %s",err, err_to_name(err));
                break;
            }
        } while (err > 0);
    }
    return NULL;
}

int tcp_client_start_handler_thread(Socket_t* connect_socket, pthread_t* handler_thread) {
    int err = 0;
    LOG_DEBUG("starting handler thread");
    err = pthread_create(handler_thread, NULL, tcp_client_handler_thread, (void*) (connect_socket));

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

    // Only values bigger than 1 byte needs to be converted
    data->sync = htons(data->sync);
    data->cmd_base = htonll(data->cmd_base);
    data->cmd = htonll(data->cmd);
    
    return 0;
}

int tcp_client_convert_data_to_host_order(tcp_client_data_t* data) {
    int err = 0;
    if(!data) {
        LOG_ERROR("data cannot be null");
        err = ERR_NULL_POINTER;
        return err;
    }

    // Only values bigger than 1 byte needs to be converted
    data->sync = ntohs(data->sync);
    data->cmd = ntohll(data->cmd);
    data->cmd_base = ntohll(data->cmd_base);

    return 0;
}

int tcp_client_send_data(tcp_client_handle_t* client_handle, tcp_client_data_t* data) {
    int err = 0;
    Socket_t connect_socket = client_handle->connect_socket;

    //before sending, convert to network order
    tcp_client_convert_data_to_network_order(data);


    err = send(connect_socket, (char*) data, sizeof(tcp_client_data_t), 0);

    if(err == INVALID_SOCKET) {
        err = errno;
        LOG_ERROR("send failed with error: %d\nerror name: %s", err, err_to_name(err));
        close(connect_socket);
        return err;
    }

    LOG_DEBUG("bytes sent: %d", err);
    return 0;
}

int tcp_client_recv_data(tcp_client_handle_t* client_handle, tcp_client_data_t* data) {
    int err = 0;
    Socket_t connect_socket = client_handle->connect_socket;
    err = recv(connect_socket, (char*) data, sizeof(tcp_client_data_t), MSG_WAITALL);

    //after receiving convert data from network to host order
    tcp_client_convert_data_to_host_order(data);

    if(err > 0) {
        LOG_DEBUG("bytes received: %d", err);
        LOG_DEBUG("data len: %u", data->data_len);
        LOG_DEBUG("data sync: %u", data->sync);
        LOG_DEBUG("data: %s", data->data);
        LOG_DEBUG("cmd base: %lu", data->cmd_base);
        LOG_DEBUG("cmd: %lu", data->cmd);
    } else if (err == 0) {
        LOG_WARN("connection %d closed", connect_socket);
        return errno;
    } else {
        err = errno;
        if(err == errno) {
            LOG_WARN("connection %d closed", connect_socket);
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
    LOG_DEBUG("closing socket %d", client_handle->connect_socket);

    err = close(client_handle->connect_socket);

    if(err == INVALID_SOCKET) {
        err = errno;
        LOG_ERROR("error when closing socket: %d\nerror name:%s", err, err_to_name(err));
        return err;
    }

    LOG_DEBUG("socket closed");

    return err;
}

uint64_t tcp_client_hash_string(char* string) {
    unsigned long hash = 5381;
    int c;
    LOG_DEBUG("string to hash: %s", string);
    while((c = (*string++))) {
        hash = ((hash << 5) + hash) + c;   /*hash * 33 + c*/
    }
    LOG_DEBUG("hash is %lu", hash);
    return hash;
}

int tcp_client_init(tcp_client_handle_t** const client_handle, const char* server_ip, uint16_t server_port) {
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