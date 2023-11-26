#include "tcp_driver.h"
#include "tcp_driver_internal.h"
#include "tcp_driver_errors.h"
#include "logger.h"
#include "memory_utils.h"
#include "err_controller.h"
#include "errors_list.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "ping/ping_sock.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

char* tcp_get_client_socket_ip(socket_t socket) {
    int err = 0;
    struct sockaddr_in addr = {0};
    socklen_t namelen = sizeof(addr);

    if(socket == INVALID_SOCKET) {
        LOG_ERROR("error %d: socket is invalid", ERR_TCP_INVALID_SOCKET);
        return NULL;
    }

    err = getpeername(socket, (struct sockaddr*)&addr, &namelen);
    if(err != 0) {
        err = errno;
        LOG_ERROR("error: %d when getting socket name: %s", err, error_to_name(err));
        return NULL;
    }
    return inet_ntoa(addr.sin_addr);
}

int tcp_get_client_socket_port(socket_t socket) {
    int err = 0;
    struct sockaddr_in addr = {0};
    socklen_t namelen = sizeof(addr);

    if(socket == INVALID_SOCKET) {
        err = ERR_TCP_INVALID_SOCKET;
        LOG_ERROR("error %d: socket is invalid", err);
        return -1;
    }

    err = getpeername(socket, (struct sockaddr*)&addr, &namelen);
    if(err != 0) {
        err = errno;
        LOG_ERROR("error %d when getting socket name: %s", err, error_to_name(err));
        return -1;
    }

    return (int)ntohs(addr.sin_port);
}

char* tcp_get_bound_socket_ip(socket_t socket) {
    int err = 0;
    struct sockaddr_in addr = {0};
    socklen_t namelen = sizeof(addr);

    if(socket == INVALID_SOCKET) {
        LOG_ERROR("error %d: socket is invalid", ERR_TCP_INVALID_SOCKET);
        return NULL;
    }

    err = getsockname(socket, (struct sockaddr*)&addr, &namelen);
    if(err != 0) {
        err = errno;
        LOG_ERROR("error %d when getting socket name: %s", err, error_to_name(err));
        return NULL;
    }
    return inet_ntoa(addr.sin_addr);
}

int tcp_get_bound_socket_port(socket_t socket) {
    int err = 0;
    struct sockaddr_in addr = {0};
    socklen_t namelen = sizeof(addr);

    if(socket == INVALID_SOCKET) {
        err = ERR_TCP_INVALID_SOCKET;
        LOG_ERROR("error %d: socket is invalid", err);
        return -1;
    }

    err = getsockname(socket, (struct sockaddr*)&addr, &namelen);
    if(err != 0) {
        err = errno;
        LOG_ERROR("error %d when getting socket name: %s", err, error_to_name(err));
        return -1;
    }

    return (int)ntohs(addr.sin_port);
}

char* tcp_get_option_name(int id) {
    switch (id)
    {
    case SO_REUSEADDR:
        return "SO_REUSEADDR";
    case SO_REUSEPORT:
        return "SO_REUSEPORT";
    case SO_KEEPALIVE:
        return "SO_KEEPALIVE";
    case SO_ACCEPTCONN:
        return "SO_ACCEPTCONN";
    case SO_DEBUG:
        return "SO_DEBUG";  
    case SO_ERROR:
        return "SO_ERROR";
    case SO_LINGER:
        return "SO_LINGER";     
    case SO_OOBINLINE:
        return "SO_OOBINLINE";
    case SO_RCVBUF:
        return "SO_RCVBUF";
    case SO_RCVLOWAT:
        return "SO_RCVLOWAT";  
    case SO_SNDBUF:
        return "SO_SNDBUF";
    case SO_TYPE:
        return "SO_TYPE";                                          
    default:
        return "not known option";
    }
}

int tcp_set_socket_option(socket_t socket, int level, int option) {
    int err = 0;
    int value = 1;

    if(setsockopt(socket, level, option, &value, sizeof(value)) != INVALID_SOCKET) {
        LOG_DEBUG("option: %s set successfully on socket: %d", tcp_get_option_name(option), socket);
    } else {
        err = errno;
        LOG_ERROR("error: %d when setting socket: %d option: %s is: %s", err, socket, tcp_get_option_name(option), error_to_name(err));
        return err;
    }
    return err;
}

int tcp_get_socket_option(socket_t socket, int level, int option) {
    int err = 0;
    int value = 0;
    socklen_t size = sizeof(value);

    if(getsockopt(socket, level, option, &value, &size) != INVALID_SOCKET) {
        LOG_DEBUG("option: %s result on socket: %d is: %d", tcp_get_option_name(option), socket, value);
        return value;
    } else {
        err = errno;
        LOG_ERROR("error: %d when setting socket: %d option: %s is: %s", err, socket, tcp_get_option_name(option), error_to_name(err));
        return err;
    }
    return err;
}

int tcp_create_socket(socket_t* sock) {
    int err = 0;
    CHECK_NULL_PTR(sock, LOG_ERROR("socket cannot be null"));

    LOG_DEBUG("preparing socket...");
    *sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(*sock == INVALID_SOCKET) {
        err = errno;
        LOG_ERROR("error when preparing socket: %s", error_to_name(err));
        return err;
    }
    LOG_DEBUG("created socket: %d", *sock);
    return err;
}

int tcp_set_reuse_addr(socket_t socket) {
    int err = 0;
    if(socket == INVALID_SOCKET) {
        LOG_ERROR("socket: %d is invalid.", socket);
        return ERR_TCP_INVALID_SOCKET;
    }

    err = tcp_set_socket_option(socket, SOL_SOCKET, SO_REUSEADDR);
    if(err != 0) {
        return err;
    }

    err = tcp_set_socket_option(socket, SOL_SOCKET, SO_REUSEPORT);
    if(err != 0) {
        return err;
    }
    LOG_DEBUG("SO_REUSEADDR and SO_REUSEPORT were set on socket: %d", socket);
    return err;
}

int tcp_prepare_address(uint16_t port, const char* address, struct sockaddr_in* addr_ipv4) {
    int err = 0;
    CHECK_NULL_PTR(address, LOG_ERROR("address cannot be null"));
    CHECK_NULL_PTR(addr_ipv4, LOG_ERROR("pointer to address structure cannot be null"));
    if(port < 1024 && port != 0) {
        LOG_ERROR("cannot use well-known ports");
        return ERR_TCP_INVALID_PORT;
    }
    if(port == 0) {
        LOG_WARN("port set to 0, using wildcard port");
    }
        
    LOG_DEBUG("preparing address...");
    err = inet_aton(address, &(addr_ipv4->sin_addr));
    if(err != 1) {
        LOG_ERROR("error when translating address to binary data");
        return ERR_TCP_ADDR_ERROR;
    }
    addr_ipv4->sin_family = AF_INET;
    addr_ipv4->sin_port = htons(port);
    LOG_DEBUG("prepared address %s:%u", address, port);
    return err;
}

int tcp_bind_socket(socket_t socket, struct sockaddr_in* addr_ipv4) {
    int err = 0;
    
    LOG_DEBUG("binding socket: %d to address: %s:%u", socket, inet_ntoa(addr_ipv4->sin_addr), addr_ipv4->sin_port);
    err = bind(socket, (struct sockaddr*)addr_ipv4, sizeof(struct sockaddr));
    if (err)
    {
        err = errno;
        LOG_ERROR("binding socket: %d failed with error: %d, error name: %s", socket, err, error_to_name(err));
        close(socket);
        return err;
    }

    LOG_DEBUG("socket: %d bound to address: %s:%u", socket, tcp_get_bound_socket_ip(socket), tcp_get_bound_socket_port(socket));
    return err;
}

int tcp_socket_listen(socket_t socket, int backlog) {
    int err = 0;
    if(socket == INVALID_SOCKET) {
        LOG_ERROR("socket cannot be invalid");
        return ERR_TCP_INVALID_SOCKET;
    }

    LOG_DEBUG("trying to listen on socket: %d", socket);
    err = listen(socket, backlog);
    if(err != 0) {
        err = errno;
        LOG_ERROR("listen on socket: %d failed with error: %d, error name: %s", socket, err, error_to_name(err));
        close(socket);
    }
    LOG_INFO("listening on socket: %s:%u", tcp_get_bound_socket_ip(socket), tcp_get_bound_socket_port(socket));
    return err;
}

int tcp_accept_client(socket_t socket, socket_t* client) {
    int err = 0;
    *client = INVALID_SOCKET;
    if(socket == INVALID_SOCKET) {
        LOG_ERROR("socket cannot be invalid");
        return ERR_TCP_INVALID_SOCKET;
    }

    *client = accept(socket, NULL, NULL);
    if (*client == INVALID_SOCKET)
    {
        err = errno;
        LOG_ERROR("error: %d during accept: %s", err, error_to_name(err));
        close(*client);
        close(socket);
        return err;
    }
    LOG_INFO("connected to socket: %s:%d", tcp_get_client_socket_ip(*client), tcp_get_bound_socket_port(*client));
    return err;
}

int tcp_connect_ipv4(socket_t socket, struct sockaddr* address) {
    int err = 0;
    socklen_t len = sizeof(*address);
    struct sockaddr_in* addr_ipv4 = NULL;
    CHECK_NULL_PTR(address, LOG_ERROR("address cannot be NULL"));
    if(socket == INVALID_SOCKET) {
        LOG_ERROR("socket cannot be invalid");
        return ERR_TCP_INVALID_SOCKET;
    }
    if(address->sa_family != AF_INET) {
        LOG_ERROR("passed not an IPv4 address");
        return ERR_TCP_ADDR_ERROR;
    }

    addr_ipv4 = (struct sockaddr_in*) address;          //cast this for debugging purposes, to use inet_ntoa()
    err = connect(socket, address, len);
    if(err != 0) {
        err = errno;
        LOG_ERROR("error: %d when connecting on socket: %d with address %s:%u, error: %s", err, socket, inet_ntoa(addr_ipv4->sin_addr), ntohs(addr_ipv4->sin_port), error_to_name(err));
        close(socket);
        return err;
    }

    LOG_INFO("connected to address %s:%u", inet_ntoa(addr_ipv4->sin_addr), ntohs(addr_ipv4->sin_port));
    return err;
}

int tcp_receive(socket_t socket, void* buf, size_t buflen) {
    int err = 0;
    CHECK_NULL_PTR(buf, LOG_ERROR("receive buffer cannot be NULL"));
    if(socket == INVALID_SOCKET) {
        LOG_ERROR("socket cannot be invalid");
        return ERR_TCP_INVALID_SOCKET;
    }
    if(buflen == 0) {
        LOG_ERROR("buffer length cannot be 0");
        return ERR_TCP_INVALID_ARGS;
    }

    err = recv(socket, buf, buflen, 0);
    if(err == -1) {
        err = errno;
        LOG_ERROR("error: %d during receiving data on socket: %d is: %s", err, socket, error_to_name(err));
    }
    
    LOG_DEBUG("received: %d bytes", err);
    return err;
}

int tcp_recv_all(socket_t socket, void* buf, size_t buflen) {
    int err = 0;
    CHECK_NULL_PTR(buf, LOG_ERROR("receive buffer cannot be NULL"));
    if(socket == INVALID_SOCKET) {
        LOG_ERROR("socket cannot be invalid");
        return ERR_TCP_INVALID_SOCKET;
    }
    if(buflen == 0) {
        LOG_ERROR("buffer length cannot be 0");
        return ERR_TCP_INVALID_ARGS;
    }

    err = recv(socket, buf, buflen, MSG_WAITALL);
    if(err == -1) {
        err = errno;
        LOG_ERROR("error: %d during receiving data on socket: %d is: %s", err, socket, error_to_name(err));
    }
    
    LOG_DEBUG("received: %d bytes on socket: %d", err, socket);
    return err;
}

int tcp_send(socket_t socket, void* data, size_t datalen) {
    int err = 0;
    CHECK_NULL_PTR(data, LOG_ERROR("receive buffer cannot be NULL"));
    if(socket == INVALID_SOCKET) {
        LOG_ERROR("socket cannot be invalid");
        return ERR_TCP_INVALID_SOCKET;
    }
    if(datalen == 0) {
        LOG_ERROR("buffer length cannot be 0");
        return ERR_TCP_INVALID_ARGS;
    }

    err = send(socket, data, datalen, 0);
    if(err == -1) {
        err = errno;
        LOG_ERROR("error: %d during send on socket: %d is: %s", err, socket, error_to_name(err));
        return err;
    }
    LOG_DEBUG("sent: %d bytes on socket: %d", err, socket);
    return err;
}