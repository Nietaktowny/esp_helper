#pragma once
#define WIN32_LEAN_AND_MEAN

#include <ws2tcpip.h>
#include <winsock2.h>
#include <pthread.h>
#include <inttypes.h>

#pragma pack(1)
struct tcp_client_data_obj {
    uint8_t data_len;
    uint16_t sync;
    uint8_t cmd_base;
    uint8_t cmd;
    char data[255];
};

typedef struct tcp_client_data_obj tcp_client_data_t;

struct tcp_client_handle_obj {
    WSADATA wsaData;
    SOCKET connect_socket;
    pthread_t handler_thread;
    struct addrinfo *result;
};

typedef struct tcp_client_handle_obj tcp_client_handle_t;


int tcp_client_init_winsock(WSADATA* wsaData);

int tcp_client_prepare_server_address(struct addrinfo** result, const char* server_addr, const char* port);

int tcp_client_set_sockopt_reuse_addr(SOCKET socket);

int tcp_client_create_connect_socket(SOCKET* connect_socket);

int tcp_client_connect_to_server(SOCKET* connect_socket, struct addrinfo** address);

int tcp_client_convert_data_to_network_order(tcp_client_data_t* data);

int tcp_client_convert_data_to_host_order(tcp_client_data_t* data);

char* tcp_client_get_server_socket_ip(SOCKET socket);

int tcp_client_get_server_socket_port(SOCKET socket);

int tcp_client_send_data(tcp_client_handle_t* client_handle, tcp_client_data_t* data);

int tcp_client_recv_data(tcp_client_handle_t* client_handle, tcp_client_data_t* data);

int tcp_client_close_connection(tcp_client_handle_t* client_handle);

int tcp_client_init(tcp_client_handle_t** client_handle, const char* server_ip, const char* server_port);

