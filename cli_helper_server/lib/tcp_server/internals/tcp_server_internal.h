#pragma once

#include "tcp_server.h"

int tcp_server_init_winsock(WSADATA* wsaData);

int tcp_server_prepare_address(struct addrinfo** result, const char* address);

int tcp_server_create_listen_socket(SOCKET* listen_socket);

int tcp_server_bind_socket(SOCKET* socket, struct addrinfo** result);

int tcp_server_listen_on_socket(SOCKET* listen_socket);

SOCKET tcp_server_accept_client(tcp_server_handle_t* server_handle);

void* tcp_server_handler_thread(void* args);

void* tcp_server_listen_thread(void* args);