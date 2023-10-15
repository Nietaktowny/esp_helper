#pragma once

#include "tcp_server.h"


int tcp_server_create_listen_socket(Socket_t* listen_socket);

int tcp_server_bind_socket(Socket_t* listen_socket, uint16_t port, const char* address);

int tcp_server_listen_on_socket(Socket_t* listen_socket);

Socket_t tcp_server_accept_client(tcp_server_handle_t* server_handle);

void* tcp_server_handler_thread(void* args);

void* tcp_server_listen_thread(void* args);