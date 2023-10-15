#pragma once

#include <pthread.h>
#include <inttypes.h>

typedef int Socket_t;

#define INVALID_SOCKET -1

#pragma pack(1)
struct tcp_client_data_obj {
    uint8_t data_len;
    uint16_t sync;
    uint64_t cmd_base;
    uint64_t cmd;
    char data[255];
};

typedef struct tcp_client_data_obj tcp_client_data_t;

struct tcp_client_handle_obj {
    Socket_t connect_socket;
    pthread_t handler_thread;
    struct sockaddr_in* result;
};

typedef struct tcp_client_handle_obj tcp_client_handle_t;


int tcp_client_prepare_server_address(struct sockaddr_in** result, const char* server_addr, uint16_t port);

int tcp_client_set_sockopt_reuse_addr(Socket_t socket);

int tcp_client_create_connect_socket(Socket_t* connect_socket);

int tcp_client_connect_to_server(Socket_t* connect_socket, struct sockaddr_in** result);

int tcp_client_convert_data_to_network_order(tcp_client_data_t* data);

int tcp_client_convert_data_to_host_order(tcp_client_data_t* data);

char* tcp_client_get_server_socket_ip(Socket_t socket);

int tcp_client_get_server_socket_port(Socket_t socket);

int tcp_client_send_data(tcp_client_handle_t* client_handle, tcp_client_data_t* data);

int tcp_client_recv_data(tcp_client_handle_t* client_handle, tcp_client_data_t* data);

int tcp_client_close_connection(tcp_client_handle_t* client_handle);

uint64_t tcp_client_hash_string(char* string);

int tcp_client_init(tcp_client_handle_t** client_handle, const char* server_ip, uint16_t server_port);

