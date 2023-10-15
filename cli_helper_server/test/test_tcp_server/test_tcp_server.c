#include "unity.h"
#include "log.h"
#include "time.h"
#include "tcp_server.h"
#include "tcp_server_internal.h"
#include "error.h"
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

SET_LOG_LEVEL(LOG_LEVEL_INFO);

/*HELPER FUNCTIONS*/
int get_random_value (int range) {
    time_t t;
    srand((unsigned) time(&t));
    return (rand() % range);
}


/*TESTS*/


void test_if_tcp_server_init_allocs_handler(void) {
    //given
    tcp_server_handle_t* test_handle = NULL;

    //when
    tcp_server_init(&test_handle, 27015, "127.0.0.1");

    //then
    TEST_ASSERT_NOT_NULL_MESSAGE(test_handle, "tcp_server_handle shouldn't be null after tcp_server_init()");

    //after
    tcp_server_deinit(&test_handle);
}

void test_if_tcp_server_handler_is_null_after_deinit(void) {
    //given
    tcp_server_handle_t* test_handle = NULL;

    //when
    tcp_server_init(&test_handle, 27015, "127.0.0.1");
    tcp_server_deinit(&test_handle);

    //then
    TEST_ASSERT_NULL_MESSAGE(test_handle, "tcp_server_handle should be null after tcp_server_deinit()");
}

void test_if_port_is_correct_after_tcp_init(void) {
    //given
    tcp_server_handle_t* test_handle = NULL;
    int random_port = get_random_value(25000);

    //when
    tcp_server_init(&test_handle, random_port, "127.0.0.1");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(random_port, test_handle->port, "Port in handle should be equal to passed to tcp init");

    //after
    tcp_server_deinit(&test_handle);
}

void test_if_tcp_server_init_returns_zero(void) {
    //given
    tcp_server_handle_t* test_handle = NULL;
    int random_port = get_random_value(25000);
    int err = -1;

    //when
    err = tcp_server_init(&test_handle, random_port, "127.0.0.1");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "tcp_server_init should return zero");

    //after
    tcp_server_deinit(&test_handle);
}


void test_if_listen_socket_is_not_invalid(void) {
    //given
    tcp_server_handle_t* test_handle = NULL;
    int random_port = 27015;
    
    //when
    tcp_server_init(&test_handle, random_port, "127.0.0.1");

    //then
    TEST_ASSERT_NOT_EQUAL_MESSAGE(INVALID_SOCKET, test_handle->listen_socket, "listen socket should not be invalid");

    //after
    tcp_server_deinit(&test_handle);
}

void test_if_listen_socket_has_reuse_option(void) {
    //given
    tcp_server_handle_t* test_handle = NULL;
    int random_port = 27015;
    // Binding to the already used socket
    int socket_option;
    socklen_t option_len = sizeof(int);
    
    //when
    tcp_server_init(&test_handle, random_port, "127.0.0.1");

    getsockopt(test_handle->listen_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&socket_option, &option_len);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(1, socket_option, "listen socket should have SO_REUSEADDR option");

    //after
    tcp_server_deinit(&test_handle);
}

void test_if_set_reuse_addr_func_works(void) {
    //given
    Socket_t test_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // Binding to the already used socket
    int socket_option;
    socklen_t option_len = sizeof(int);
    
    //when
    tcp_server_set_sockopt_reuse_addr(test_socket);

    //then
    getsockopt(test_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&socket_option, &option_len);
    TEST_ASSERT_EQUAL_MESSAGE(1, socket_option, "listen socket should have SO_REUSEADDR option");
}

void test_if_listen_socket_has_correct_ip(void) {
    //given
    tcp_server_handle_t* test_handle = NULL;
    int random_port = 27015;
    char* test_ip = "127.27.32.57";
    
    //when
    tcp_server_init(&test_handle, random_port, test_ip);

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_ip, tcp_server_get_server_socket_ip(test_handle->listen_socket), "listen socket IP should be as specified");

    //after
    tcp_server_deinit(&test_handle);    
}

void test_if_listen_socket_has_correct_port(void) {
    //given
    tcp_server_handle_t* test_handle = NULL;
    int random_port = 27015;
    char* test_ip = "127.27.32.57";
    
    //when
    tcp_server_init(&test_handle, random_port, test_ip);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(random_port, tcp_server_get_server_socket_port(test_handle->listen_socket), "listen socket port should be as specified");

    //after
    tcp_server_deinit(&test_handle);    
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_if_tcp_server_init_allocs_handler);
    RUN_TEST(test_if_tcp_server_handler_is_null_after_deinit);
    RUN_TEST(test_if_port_is_correct_after_tcp_init);
    RUN_TEST(test_if_tcp_server_init_returns_zero);
    RUN_TEST(test_if_listen_socket_is_not_invalid);
    RUN_TEST(test_if_listen_socket_has_reuse_option);
    RUN_TEST(test_if_set_reuse_addr_func_works);
    RUN_TEST(test_if_listen_socket_has_correct_ip);
    RUN_TEST(test_if_listen_socket_has_correct_port);

    return UNITY_END();
}