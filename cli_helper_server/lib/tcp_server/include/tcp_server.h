#pragma once
#define WIN32_LEAN_AND_MEAN

#include <ws2tcpip.h>
#include <winsock2.h>
#include <pthread.h>
#include <inttypes.h>

#pragma pack(1)
struct tcp_server_data_obj {
    uint8_t data_len;
    uint16_t sync;
    uint8_t cmd_base;
    uint8_t cmd;
    char data[255];
};

typedef struct tcp_server_data_obj tcp_server_data_t;



struct tcp_server_handle_obj {
    WSADATA wsaData;
    SOCKET listen_socket;
    pthread_t listen_thread;
    pthread_t handler_thread;
    int port;
    struct addrinfo *result;
};

typedef struct tcp_server_handle_obj tcp_server_handle_t;


#define TCP_BACKLOG         4
#define TCP_BLOCK_MODE      0
#define TCP_NONBLOCK_MODE   1


int tcp_server_toggle_socket_block_mode(SOCKET socket, int mode_val);

int tcp_server_set_sockopt_reuse_addr(SOCKET socket);

char* tcp_server_get_server_socket_ip(SOCKET socket);

int tcp_server_get_server_socket_port(SOCKET socket);

char* tcp_server_get_client_socket_ip(SOCKET socket);

int tcp_server_get_client_socket_port(SOCKET socket);

int tcp_server_init(tcp_server_handle_t** server_handle, int listen_port, const char* ip_address);

int tcp_server_deinit(tcp_server_handle_t** server_handle);

int tcp_server_start_listen_thread(tcp_server_handle_t* server_handle);

int tcp_server_start_handler_thread(SOCKET client_socket, tcp_server_handle_t* server_handle);