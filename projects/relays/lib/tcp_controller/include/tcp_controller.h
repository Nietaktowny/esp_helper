/**
 * @file tcp_controller.h
 * @author Wojciech Mytych
 * @brief TCP Controller header file.
 * @version 1.2.3
 * @date 2024-02-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "lwip/inet.h"
#include "lwip/sockets.h"

typedef int socket_t;

/**
 * @brief Create socket.
 * 
 * @param socket_t*[out] pointer to where socket will be stored
 * 
 * @retval 0 on success
 * @retval ERR_C_NULL_POINTER on NULL socket_t* pointer
 * @retval reads errno on other errors
*/
int tcp_create_socket(socket_t* sock);

/**
 * @brief Prepare address for binding socket.
 * 
 * @warning When port is set to 0, wildcard port will be used.
 * 
 * @param uint16_t[in] address port
 * @param char*[in] IPv4 address
 * @param sockaddr_in*[out] pointer to sockaddr_in structure where address result will be stored
 * 
 * @retval 0 on success
 * @retval ERR_C_NULL_POINTER when address/addr_ipv4 was NULL
 * @retval ERR_TCP_ADDR_ERROR when address was invalid
 * @retval ERR_TCP_INVALID_PORT when trying to use port < 1024
*/
int tcp_prepare_address(uint16_t port, const char* address, struct sockaddr_in* addr_ipv4);

/**
 * @brief Read IPv4 address to which socket is bound.
 * 
 * @warning This function should only be called on bound sockets.
 * 
 * @param socket_t socket descriptor
 * 
 * @retval har* string containing IPv4 address
 * @retval NULL on error
*/
char* tcp_get_bound_socket_ip(socket_t socket);

/**
 * @brief Read port to which socket is bound.
 * 
 * @warning This function should only be called on bound sockets.
 * 
 * @param socket_t socket descriptor
 * 
 * @retval int value containing socket port on success
 * @retval -1 on error (error value is logged)
*/
int tcp_get_bound_socket_port(socket_t socket);

/**
 * @brief Bind socket to prepared address.
 * 
 * @param socket_t socket descriptor
 * @param sockaddr_in* prepared address
 * 
 * @retval 0 on success
 * @retval errno value on error
 * 
*/
int tcp_bind_socket(socket_t socket, struct sockaddr_in* addr_ipv4);

/**
 * @brief Listen on socket for incoming connections.
 * 
 * @param socket socket descriptor
 * @param backlog  number of pending connections
 * 
 * @note backlog can be ignored by system, see https://www.linuxjournal.com/files/linuxjournal.com/linuxjournal/articles/023/2333/2333s2.html
 * 
 * @retval 0 on success
 * @retval ERR_TCP_INVALID_SOCKET on invalid listen socket
 * @retval errno values on other errors
*/
int tcp_socket_listen(socket_t socket, int backlog);

/**
 * @brief Accept connection on listen socket, and return client socket.
 * 
 * @param socket[in] listening socket descriptor
 * @param client[out] new client connected socket descriptor
 * 
 * @retval 0 on success
 * @retval ERR_TCP_INVALID_SOCKET on invalid listen socket
 * @retval errno values on other errors
*/
int tcp_accept_client(socket_t socket, socket_t* client);

/**
 * @brief Translate socket option integer value to string name.
 * 
 * @note Not all socket options are currently translated.
 * 
 * @param id integer value of socket option
 * 
 * @retval char* option name string
 * @reval char* "not known option" when no option is matched
*/
char* tcp_get_option_name(int id);

/**
 * @brief Set socket option.
 * 
 * @param socket socket descriptor
 * @param level  option level (for example SOL_SOCKET)
 * @param option option to set (for example SO_REUSEADDR)
 * 
 * @retval 0 on success
 * @retval errno values on error   
*/
int tcp_set_socket_option(socket_t socket, int level, int option);

/**
 * @brief Get socket option.
 * 
 * @param socket socket descriptor
 * @param level  option level (for example SOL_SOCKET)
 * @param option option to get (for example SO_REUSEADDR)
 * 
 * @retval 0 on success
 * @retval errno values on error   
*/
int tcp_get_socket_option(socket_t socket, int level, int option);

/**
 * @brief Set SO_REUSEADDR option on socket.
 * 
 * @param socket socket descriptor
 * 
 * @retval 0 on success
 * @retval ERR_TCP_INVALID_SOCKET on invalid socket
 * @retval errno on other errors
*/
int tcp_set_reuse_addr(socket_t socket);

/**
 * @brief Connect to socket using IPv4 address.
 * 
 * @param socket socket descriptor
 * @param address address to connect to
 * 
 * @retval 0 on success
 * @retval ERR_TCP_ADDR_ERROR on invalid address
 * @retval errno on other errors
*/
int tcp_connect_ipv4(socket_t socket, struct sockaddr* address);

/**
 * @brief Receive data from socket.
 *  
 * 
 * @param socket socket descriptor
 * @param buf buffer to store data
 * @param buflen size of buffer
 * 
 * @retval 0 on success
 * @retval ERR_TCP_INVALID_SOCKET on invalid socket
 * @retval ERR_TCP_INVALID_ARGS on invalid arguments
 * @retval errno on other errors
*/
int tcp_receive(socket_t socket, void* buf, size_t buflen);

/**
 * @brief Receive data from socket, but read only on full buffer.
 * 
 * @note This function uses MSG_WAITALL option.
 * 
 * @param socket socket descriptor
 * @param buf buffer to store data
 * @param buflen size of buffer
 * 
 * @retval 0 on success
 * @retval ERR_TCP_INVALID_SOCKET on invalid socket
 * @retval ERR_TCP_INVALID_ARGS on invalid arguments
 * @retval errno on other errors
*/
int tcp_recv_all(socket_t socket, void* buf, size_t buflen);

/**
 * @brief Send data using socket.
 * 
 * @param socket socket descriptor
 * @param data data to send
 * @param datalen length of data to send
 * 
 * @retval 0 on success
 * @retval ERR_TCP_INVALID_SOCKET on invalid socket
 * @retval ERR_TCP_INVALID_ARGS on invalid arguments
 * @retval errno on other errors
 * 
*/
int tcp_send(socket_t socket, void* data, size_t datalen);


