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
#include <limits.h>

SET_LOG_LEVEL(LOG_LEVEL_INFO);

#define TEST_PORT 27015
#define TEST_ADDRESS "127.0.0.1"

/*HELPER FUNCTIONS*/
int get_random_value (int range) {
    time_t t;
    srand((unsigned) time(&t));
    return (rand() % range);
}


/*TESTS*/

void test_if_server_handle_is_null_after_init(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);

    //then
    TEST_ASSERT_NOT_NULL_MESSAGE(server_handle, "tcp server handle should not be null after init");
}

void test_if_pointer_to_server_handle_is_null_after_init(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);

    //then
    TEST_ASSERT_NOT_NULL_MESSAGE(&server_handle, "pointer to tcp server handle should not be null after init");
}

void test_if_tcp_server_init_returns_zero(void) {
    //given
    int err = -1;
    tcp_server_handle_t* server_handle = NULL;

    //when
    err = tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "tcp_server_init should return 0");
}

void test_if_listen_socket_is_created(void) {
    //given
    tcp_server_handle_t* server_handle;

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);

    //then
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, server_handle->listen_socket, "listen socket should not be 0 after initializing");
}

void test_if_reuse_address_error_is_not_returned_upon_multiple_inits(void) {
    //given
    int err = -1;
    int ADDRESS_ALREADY_USED_ERR = 98;
    tcp_server_handle_t* server_handle = NULL;

    //when
    err = tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    err = tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS); //trying to bind there socket for second time to the same address and port

    //then
    TEST_ASSERT_NOT_EQUAL_MESSAGE(ADDRESS_ALREADY_USED_ERR, err, "getting 'Address already in use' error");
    
}

void test_if_head_is_null_after_init(void) {
    //given
    tcp_server_handle_t* test_handle = NULL;

    //when
    tcp_server_init(&test_handle, 27015, "127.0.0.1");

    //then
    TEST_ASSERT_NULL_MESSAGE(test_handle->list.head, "list head should be null after initializing");
}

void test_if_tail_is_null_after_init(void) {
    //given
    tcp_server_handle_t* test_handle = NULL;

    //when
    tcp_server_init(&test_handle, 27015, "127.0.0.1");

    //then
    TEST_ASSERT_NULL_MESSAGE(test_handle->list.tail, "list tail should be null after initializing");
}

void test_if_listen_thread_is_zero_after_init(void) {
    //given
    tcp_server_handle_t* test_handle = NULL;

    //when
    tcp_server_init(&test_handle, TEST_PORT, TEST_ADDRESS);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, test_handle->listen_thread, "listen thread should be 0 after init");
}

void test_if_handler_thread_is_zero_after_init(void) {
    //given
    tcp_server_handle_t* test_handle = NULL;

    //when
    tcp_server_init(&test_handle, TEST_PORT, TEST_ADDRESS);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, test_handle->handler_thread, "handler thread should be 0 after init");
}

void test_if_reuse_addr_opt_is_set_on_socket(void) {
    //given
    int err = -1;
    int option = -123;
    socklen_t len = sizeof(option);
    tcp_server_handle_t* server_handle = NULL;

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    err = getsockopt(server_handle->listen_socket, SOL_SOCKET, SO_REUSEADDR, &option, &len);
    if(err != 0) {
        LOG_ERROR("error in test_if_reuse_addr_opt_is_set_on_socket: %s:%d",strerror(errno) ,errno);
        TEST_FAIL_MESSAGE("error when trying to getsockopt");
    }

    //then
    TEST_ASSERT_EQUAL_MESSAGE(1, option, "reuse addr should be set to 1 on listen socket");
}

void test_if_reuse_port_opt_is_set_on_socket(void) {
    //given
    int err = -1;
    int option = -123;
    socklen_t len = sizeof(option);
    tcp_server_handle_t* server_handle = NULL;

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    err = getsockopt(server_handle->listen_socket, SOL_SOCKET, SO_REUSEPORT, &option, &len);
    if(err != 0) {
        LOG_ERROR("error in test_if_reuse_port_opt_is_set_on_socket: %s:%d",strerror(errno) ,errno);
        TEST_FAIL_MESSAGE("error when trying to getsockopt");
    }

    //then
    TEST_ASSERT_EQUAL_MESSAGE(1, option, "reuse addr should be set to 1 on listen socket");
}

void test_if_init_returns_err_on_wrong_address(void) {
    //given
    const char* wrong_addr = "12...asdads";
    tcp_server_handle_t* server_handle = NULL;

    //when
    int err = tcp_server_init(&server_handle, TEST_PORT, wrong_addr);

    
    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_TCPS_WRONG_ADDR, err, "tcp_server_init should return err when wrong address was passed");
}

void test_if_init_returns_err_on_null_handle(void) {
    //given
    int err = 0;

    //when
    err = tcp_server_init(NULL, TEST_PORT, TEST_ADDRESS);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "null pointer error was not returned");
}

void test_if_init_not_returns_err_on_zero_port(void) {
    //given
    const int zero_port = 0;
    tcp_server_handle_t* server_handle = NULL;

    //when
    int err = tcp_server_init(&server_handle, zero_port, TEST_ADDRESS);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "tcp_server_init should not return err when using wildcart port");
}

void test_if_init_returns_err_on_well_known_port(void) {
    //given
    const int well_known_port = 80;
    tcp_server_handle_t* server_handle = NULL;

    //when
    int err = tcp_server_init(&server_handle, well_known_port, TEST_ADDRESS);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_TCPS_WRONG_PORT, err, "tcp_server_init should return err when well known port was passed");
}

void test_if_init_returns_zero_on_max_port_value(void) {
    //given
    const int max_port = UINT16_MAX;
    tcp_server_handle_t* server_handle = NULL;

    //when
    int err = tcp_server_init(&server_handle, max_port, TEST_ADDRESS);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "tcp_server_init should not return err when binding to last port");
}

void test_size_of_allocated_memory_for_handle(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;
    int sizeof_pointer = 0;
    int sizeof_struct = 0;

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    sizeof_pointer = sizeof(*server_handle);
    sizeof_struct = sizeof(tcp_server_handle_t);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(sizeof_struct, sizeof_pointer, "sizeof pointer to handle should be the same as handle struct");
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_if_server_handle_is_null_after_init);
    RUN_TEST(test_if_pointer_to_server_handle_is_null_after_init);
    RUN_TEST(test_if_tcp_server_init_returns_zero);
    RUN_TEST(test_if_listen_socket_is_created);
    RUN_TEST(test_if_reuse_address_error_is_not_returned_upon_multiple_inits);
    RUN_TEST(test_if_head_is_null_after_init);
    RUN_TEST(test_if_tail_is_null_after_init);
    RUN_TEST(test_if_listen_thread_is_zero_after_init);
    RUN_TEST(test_if_handler_thread_is_zero_after_init);
    RUN_TEST(test_if_reuse_addr_opt_is_set_on_socket);
    RUN_TEST(test_if_reuse_port_opt_is_set_on_socket);
    RUN_TEST(test_if_init_returns_err_on_wrong_address);
    RUN_TEST(test_if_init_returns_err_on_null_handle);
    RUN_TEST(test_if_init_not_returns_err_on_zero_port);
    RUN_TEST(test_if_init_returns_err_on_well_known_port);
    RUN_TEST(test_if_init_returns_zero_on_max_port_value);
    RUN_TEST(test_size_of_allocated_memory_for_handle);
    return UNITY_END();
}