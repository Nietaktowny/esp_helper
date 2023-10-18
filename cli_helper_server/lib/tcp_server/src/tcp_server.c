#include "tcp_server.h"
#include "tcp_server_internal.h"
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
#include <fcntl.h>
#include <arpa/inet.h>



#define DEFAULT_BUFLEN 512

#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887" 



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

int tcp_server_create_listen_socket(Socket_t* listen_socket) {
    int err = 0;

    if(!listen_socket) {
        LOG_WARN("connect_socket cannot be null, pointer");
        err = ERR_NULL_POINTER;
        return err;
    }

    LOG_DEBUG("Creating listen socket");
    
    *listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    err = tcp_server_set_sockopt_reuse_addr(*listen_socket);
    err = tcp_server_set_sockopt_reuse_port(*listen_socket);

    if(*listen_socket == INVALID_SOCKET) {
        err = errno;
        LOG_FATAL("socket failed with error %d", err);
    }

    return err;
}

int tcp_server_bind_socket(Socket_t* listen_socket, uint16_t port, const char* address) {
    int err = 0;
    LOG_DEBUG("preparing to bind socket to address: %s:%u", address, port);

    if(port != 0 && port < 1024) {
        LOG_ERROR("cannot bind to well-known port");
        err = ERR_TCPS_WRONG_PORT;
        return err;
    } else if (!port)
    {
        LOG_WARN("using wildcard port");
    }
    

    struct sockaddr_storage prep_server_addr = {0};
    memset(&prep_server_addr, 0, sizeof(prep_server_addr));
    struct sockaddr_in* server_addr_ip4 = {(struct sockaddr_in*)&prep_server_addr};
    server_addr_ip4->sin_family = AF_INET;
    server_addr_ip4->sin_port = htons(port);
    
    err = inet_aton(address, &(server_addr_ip4->sin_addr));
    if(err != 1) {
        LOG_ERROR("error when translating address to binary data");
        return ERR_TCPS_WRONG_ADDR;
    }
    

    LOG_DEBUG("binding socket");
    err = bind(*listen_socket, (struct sockaddr*) server_addr_ip4, sizeof(struct sockaddr));

    if(err == INVALID_SOCKET) {
        err = errno;
        LOG_ERROR("bind failed with error %d,\nerror name: %s", err, err_to_name(err));
        close(*listen_socket);
        return err;
    }

    LOG_INFO("bound socket to address: %s:%d", tcp_server_get_server_socket_ip(*listen_socket), tcp_server_get_server_socket_port(*listen_socket));
    
    return err;
}

int tcp_server_listen_on_socket(Socket_t* listen_socket) {
    int err = 0;
    LOG_DEBUG("listening on socket");

    if(*listen_socket == INVALID_SOCKET) {
        LOG_ERROR("Socket cannot be invalid");
        return EBADF;
    }

    err = listen(*listen_socket, TCP_BACKLOG);

    if(err == INVALID_SOCKET) {
        err = errno;
        LOG_ERROR("listen failed with error: %d", err);
        LOG_ERROR("Error name: %s", err_to_name(err));
        close(*listen_socket);
        return -1;
    }

    return 0;
}

Socket_t tcp_server_accept_client(tcp_server_handle_t* server_handle) {
    int err = 0;
    LOG_DEBUG("waiting for accept on listen socket");

    if(server_handle->listen_socket == INVALID_SOCKET) {
        LOG_ERROR("Socket cannot be invalid");
        return EBADF;
    }

    Socket_t client_socket = accept(server_handle->listen_socket, NULL, NULL);

    if(client_socket == INVALID_SOCKET) {
        err = errno;
        LOG_ERROR("accept failed with error: %d", err);
        LOG_ERROR("Error name: %s", err_to_name(err));
        close(server_handle->listen_socket);
        return 1;
    }

    LOG_DEBUG("new connection accepted");
    LOG_DEBUG("connected to socket: %s:%d", tcp_server_get_client_socket_ip(client_socket), tcp_server_get_client_socket_port(client_socket));
    return client_socket;
}

int tcp_server_toggle_socket_block_mode(Socket_t socket) {
    int err = 0;

   err = fcntl(socket, F_SETFL, O_NONBLOCK);

    if(err != 0) {
        LOG_ERROR("ioctlsocket failed with error: %d", err);
    }

    return err;
}

int tcp_server_set_sockopt_reuse_addr(Socket_t socket) {
    int err = 0;
    int option_value = 1;

    /*
     * turn off bind address checking, and allow
     * port numbers to be reused - otherwise
     * the TIME_WAIT phenomenon will prevent
     * binding to these address.port combinations
     * for (2 * MSL) seconds.
     */

    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&option_value, sizeof(option_value)) != INVALID_SOCKET) {
        LOG_DEBUG("socket can be bound to an address that is already in use.");
    } else {
        err = errno;
        LOG_ERROR("error when setting SO_REUSEADDR option: %d,\nerror name: %s", err, err_to_name(err));
        return err;
    }
    return err;
}

int tcp_server_set_sockopt_reuse_port(Socket_t socket) {
    int err = 0;
    int option_value = 1;

    /*
     * turn off bind address checking, and allow
     * port numbers to be reused - otherwise
     * the TIME_WAIT phenomenon will prevent
     * binding to these address.port combinations
     * for (2 * MSL) seconds.
     */

    if (setsockopt(socket, SOL_SOCKET, SO_REUSEPORT, (const char*)&option_value, sizeof(option_value)) != INVALID_SOCKET) {
        LOG_DEBUG("socket can be bound to an port that is already in use.");
    } else {
        err = errno;
        LOG_ERROR("error when setting SO_REUSEPORT option: %d,\nerror name: %s", err, err_to_name(err));
        return err;
    }
    return err;
}

char* tcp_server_get_server_socket_ip(Socket_t socket) {
    int err = 0;
    struct sockaddr_in socket_addr;
    socklen_t namelen = sizeof(socket_addr);
    
    memset(&socket_addr, 0, sizeof(socket_addr));

    err = getsockname(socket, (struct sockaddr*) &socket_addr, &namelen);

    if(err != 0) {
        err = errno;
        LOG_ERROR("error when getting server socket name %d,\nerror name: %s", err, err_to_name(err));
    }

    return inet_ntoa(socket_addr.sin_addr);
}

int tcp_server_get_server_socket_port(Socket_t socket) {
    int err = 0;
    struct sockaddr_in socket_addr;
    socklen_t namelen = sizeof(struct sockaddr_in);
    
    memset(&socket_addr, 0, sizeof(socket_addr));

    err = getsockname(socket, (struct sockaddr*) &socket_addr, &namelen);

    if(err != 0) {
        err = errno;
        LOG_ERROR("error when getting server socket name %d,\nerror name: %s", err, err_to_name(err));
        return -1;
    }

    return (int)ntohs(socket_addr.sin_port);
}

char* tcp_server_get_client_socket_ip(Socket_t socket) {
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

int tcp_server_get_client_socket_port(Socket_t socket) {
    int err = 0;
    struct sockaddr_in socket_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    
    memset(&socket_addr, 0, sizeof(socket_addr));

    err = getpeername(socket,(struct sockaddr*) &socket_addr, &addr_len);

    if(err != 0) {
        err = errno;
        LOG_ERROR("error when getting client socket port %d,\nerror name: %s", err, err_to_name(err));
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
    data->cmd_base = htonll(data->cmd_base);
    data->cmd = htonll(data->cmd);
    
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
    data->cmd_base = ntohll(data->cmd_base);
    data->cmd = ntohll(data->cmd);
    
    return 0;
}

int tcp_server_recv_data(Socket_t client_socket, tcp_server_data_t* data) {
    int err = 0;
    err = recv(client_socket, (char*) data, sizeof(tcp_server_data_t), MSG_WAITALL);

    if(err > 0) {
        LOG_DEBUG("bytes received: %d", err);
        LOG_DEBUG("data len: %u", data->data_len);
        LOG_DEBUG("data sync: %u", data->sync);
        LOG_DEBUG("data: %s", data->data);
        LOG_DEBUG("cmd base: %lu", data->cmd_base);
        LOG_DEBUG("cmd: %lu", data->cmd);
    } else if (err == 0) {
        LOG_WARN("connection %d closed", client_socket);
        return EBADF;
    } else {
        err = errno;
        if(err == EBADF) {
            LOG_WARN("connection %d closed", client_socket);
            return err;
        } else {
            LOG_ERROR("recv failed with error: %d\nerror name: %s",err, err_to_name(err));
        }
        return err;
    }
    return 0;
}

uint64_t tcp_server_hash_string(char* string) {
    uint64_t hash = 5381;
    int c;

    if(!string) {
        LOG_ERROR("string pointer cannot be null");
        return 0;
    }

    LOG_DEBUG("string to hash: %s", string);
    while((c = (*string++))) {
        hash = ((hash << 5) + hash) + c;   /*hash * 33 + c*/
    }
    LOG_DEBUG("hash is %lu", hash);
    return hash;
}

tcp_server_cmd_t* tcp_server_find_string_cmd(tcp_server_handle_t** server_handle, char* cmd_base, char* cmd) {
    if(!server_handle || !(*server_handle)) {
        LOG_ERROR("server_handle cannot be null");
        return NULL;
    }
    
    if(!cmd_base) {
        LOG_ERROR("cmd_base cannot be null");
        return NULL;
    }

    if(!cmd_base) {
        LOG_ERROR("cmd_base cannot be null");
        return NULL;
    }

    tcp_server_cmd_node_t* iterator = (*server_handle)->list.tail;
    uint64_t fcmd_base = tcp_server_hash_string(cmd_base);
    uint64_t fcmd = tcp_server_hash_string(cmd);

    while (iterator != NULL)
    {
        if(iterator->cmd.cmd_base == fcmd_base && iterator->cmd.cmd == fcmd) {
            LOG_DEBUG("command with base %lu and cmd %lu found", iterator->cmd.cmd_base, iterator->cmd.cmd);
            return &(iterator->cmd);
        }
        iterator = iterator->next;
    }
    LOG_DEBUG("command not found");
    return NULL;
}

tcp_server_cmd_t* tcp_server_find_hashed_cmd(tcp_server_handle_t** server_handle, uint64_t fcmd_base, uint64_t fcmd) {
    if(!server_handle || !(*server_handle)) {
        LOG_ERROR("server_handle cannot be null");
        return NULL;
    }

    tcp_server_cmd_node_t* iterator = (*server_handle)->list.tail;
    while (iterator != NULL)
    {
        if(iterator->cmd.cmd_base == fcmd_base && iterator->cmd.cmd == fcmd) {
            LOG_DEBUG("command with base %lu and cmd %lu found", iterator->cmd.cmd_base, iterator->cmd.cmd);
            return &(iterator->cmd);
        }
        iterator = iterator->next;
    }
    LOG_DEBUG("command not found");
    return NULL;
}

int tcp_server_register_cmd(tcp_server_handle_t** server_handle, void* (*cmd_fun)(void*), char* cmd_base, char* cmd) {
    int err = 0;

    if(!server_handle || !(*server_handle)) {
        err = ERR_NULL_POINTER;
        LOG_FATAL("server handle cannot be null");
        return err;
    }

    if(!cmd_fun) {
        err = ERR_NULL_POINTER;
        LOG_FATAL("command function cannot be null");
        return err;  
    }

    if(!cmd_base || !cmd) {
        err = ERR_NULL_POINTER;
        LOG_FATAL("command or command base cannot be null");
        return err;
    }
    uint64_t h_cmd_base = tcp_server_hash_string(cmd_base);
    uint64_t h_cmd = tcp_server_hash_string(cmd);

    if(tcp_server_find_hashed_cmd(server_handle, h_cmd_base, h_cmd) != NULL) {
        LOG_WARN("such cmd already registered");
        err = ERR_TCPS_DUPLICATE_CMD;
        return err;
    }

    tcp_server_cmd_node_t* current = NULL;

    current = malloc(sizeof(tcp_server_cmd_node_t));
    current->cmd.cmd = h_cmd;
    current->cmd.cmd_base = h_cmd_base;
    current->cmd.cmd_fun = cmd_fun;
    current->next = NULL;

    if((*server_handle)->list.tail != NULL && (*server_handle)->list.head != NULL) {
        /*If there is some tail then this isn't first node to be created*/
        //update head
        (*server_handle)->list.head->next = current;
        (*server_handle)->list.head = current;
    } else {
        /*If tail ist null, then this may be the first node to be created*/
        // Init tail and head
        (*server_handle)->list.head = current;
        (*server_handle)->list.tail = current;
    }

    return err;
}

int tcp_server_send_cmd(tcp_server_handle_t** server_handle) {
    int err = 0;

    return err;
}

void* tcp_server_handler_thread(void* args) {
    int err = 0;
    tcp_server_data_t data;
    Socket_t client_socket = (Socket_t) args;
    LOG_DEBUG("handling socket transmission: %d\n", client_socket);
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
            LOG_DEBUG("data cmd_base: %lu", data.cmd_base);
            LOG_DEBUG("data cmd: %lu", data.cmd);
            LOG_DEBUG("data: %s", data.data);
        } else if (err == 0) {
            LOG_DEBUG("connection %d closed", client_socket);
            break;
        } else {
            err = errno;

            if(err == EBADF) {
                LOG_WARN("%s", err_to_name(err));
                break;
            }
            LOG_ERROR("recv failed with error: %d\nerror name: %s",err, err_to_name(err));
            close(client_socket);
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

        if(err == INVALID_SOCKET) {
            err = errno;
            LOG_ERROR("send failed with error: %d\nerror name: %s", err, err_to_name(err));
            close(client_socket);
            pthread_exit(NULL);
        }

        LOG_DEBUG("bytes sent: %d", err);

        err = 0;
    }
    LOG_DEBUG("finished transmission");
    close(client_socket);
    LOG_DEBUG("closed socket: %d", client_socket);
    return NULL;
}

int tcp_server_start_handler_thread(Socket_t client_socket, tcp_server_handle_t* server_handle) {
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
        Socket_t new_client_socket = tcp_server_accept_client(server_handle);
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

int tcp_server_init(tcp_server_handle_t** const server_handle, uint16_t listen_port, const char* ip_address) {
    int err = 0;

    if(!server_handle) {
        LOG_FATAL("tcp server handle cannot be null");
        err = ERR_NULL_POINTER;
        return err;
    }

    LOG_DEBUG("allocating memory for tcp_server...");
    *server_handle = (tcp_server_handle_t*) malloc(sizeof(tcp_server_handle_t));

    /*Initialize handle variables*/
    (*server_handle)->list.head = NULL;
    (*server_handle)->list.tail = NULL;
    (*server_handle)->listen_socket = 0;
    (*server_handle)->listen_thread = 0;
    (*server_handle)->handler_thread = 0;

    if(!(*server_handle)) {
        err = ERR_NO_MEMORY;
        LOG_FATAL("no memory for tcp server");
        return err;
    }

    err = tcp_server_create_listen_socket(&((*server_handle)->listen_socket));
    if(err != 0) {
        return err;;
    }

    err = tcp_server_bind_socket(&((*server_handle)->listen_socket), listen_port, ip_address);

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

    err = shutdown((*server_handle)->listen_socket, SHUT_RDWR);
    if(err != 0) {
        err= errno;
        if(err != 0) {
            LOG_ERROR("error when trying to shutdown the socket: %d,\nerror name: %s", err, err_to_name(err));
            return err;
        } else {
            LOG_DEBUG("trying to shutdown not connected socket");
        }
    }

    err = close((*server_handle)->listen_socket);
    if(err != 0 && err != 0) {
        err = errno;
        if(err != 0) {
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

    memset(*server_handle, 0, sizeof(tcp_server_handle_t));
    free(*server_handle);
    *server_handle = NULL;
    return err;
}