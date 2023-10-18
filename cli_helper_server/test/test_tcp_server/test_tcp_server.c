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

SET_LOG_LEVEL(LOG_LEVEL_DEBUG);

#define TEST_PORT 27015
#define TEST_ADDRESS "127.0.0.1"

/*HELPER FUNCTIONS*/
int get_random_value (int range) {
    time_t t;
    srand((unsigned) time(&t));
    return (rand() % range);
}

void* test_cmd_fun(void* args) {
    LOG_DEBUG("Iam test function");
    return NULL;
}

void* test_other_fun(void* args) {
    LOG_DEBUG("Iam other test function");
    return NULL;
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

void test_if_head_is_not_null_after_adding_node(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    tcp_server_register_cmd(&server_handle, test_cmd_fun, "test_base", "test_cmd");

    //then
    TEST_ASSERT_NOT_NULL_MESSAGE(server_handle->list.head, "head should not be null after adding node");
}

void test_if_tail_is_not_null_after_adding_node(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    tcp_server_register_cmd(&server_handle, test_cmd_fun, "test_base", "test_cmd");

    //then
    TEST_ASSERT_NOT_NULL_MESSAGE(server_handle->list.tail, "tail should not be null after adding node");
}

void test_if_head_equals_tail_after_adding_one_node(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    tcp_server_register_cmd(&server_handle, test_cmd_fun, "test_base", "test_cmd");

    //then
    TEST_ASSERT_EQUAL_PTR_MESSAGE(server_handle->list.head, server_handle->list.tail, "tail should equal head after adding one node");
}

void test_if_register_cmd_returns_zero_on_correct_params(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;
    int err = -1;

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    err = tcp_server_register_cmd(&server_handle, test_cmd_fun, "test_base", "test_cmd");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "tcp_server_register_cmd should return 0 on correct parameters");
}

void test_if_register_cmd_returns_err_on_null_handle(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;
    int err = -1;

    //when
    err = tcp_server_register_cmd(&server_handle, test_cmd_fun, "test_base", "test_cmd");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "tcp_server_register_cmd should return ERR_NULL_POINTER on null handle");
}

void test_if_register_cmd_returns_err_on_null_cmd_base(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;
    int err = -1;

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    err = tcp_server_register_cmd(&server_handle, test_cmd_fun, NULL, "test_cmd");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "tcp_server_register_cmd should return ERR_NULL_POINTER on null cmd_base");
}

void test_if_register_cmd_returns_err_on_null_cmd(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;
    int err = -1;

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    err = tcp_server_register_cmd(&server_handle, test_cmd_fun, "test_base", NULL);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "tcp_server_register_cmd should return ERR_NULL_POINTER on null cmd");
}

void test_if_register_cmd_returns_err_on_null_function(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;
    int err = -1;

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    err = tcp_server_register_cmd(&server_handle, NULL, "test_base", "test_cmd");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "tcp_server_register_cmd should return ERR_NULL_POINTER on null function");
}

void test_if_hash_cmd_base_of_new_node_is_correct(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;
    char* test_cmd_base = "test_cmd_base";
    uint64_t cmd_base_hash = tcp_server_hash_string(test_cmd_base);

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    tcp_server_register_cmd(&server_handle, test_cmd_fun, test_cmd_base, "test_cmd");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(cmd_base_hash, server_handle->list.head->cmd.cmd_base, "hash of cmd_base different than expected");
}

void test_if_hash_cmd_of_new_node_is_correct(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;
    char* test_cmd = "test_cmd";
    uint64_t cmd_hash = tcp_server_hash_string(test_cmd);

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    tcp_server_register_cmd(&server_handle, test_cmd_fun, "test_cmd_base", test_cmd);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(cmd_hash, server_handle->list.head->cmd.cmd, "hash of cmd different than expected");
}

void test_if_fun_pointer_of_new_node_is_correct(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;
    void* (*test_fun)(void*) = test_cmd_fun;

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    tcp_server_register_cmd(&server_handle, test_cmd_fun, "test_cmd_base", "test_cmd");

    //then
    TEST_ASSERT_EQUAL_PTR_MESSAGE(test_fun, server_handle->list.head->cmd.cmd_fun, "test function of new node different than expected");
}

void test_if_head_is_different_than_head_on_two_registered_cmd(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    tcp_server_register_cmd(&server_handle, test_cmd_fun, "test_cmd_base", "test_cmd");
    tcp_server_register_cmd(&server_handle, test_cmd_fun, "dif_test_base", "dif_test_cmd");

    //then
    TEST_ASSERT_NOT_EQUAL_MESSAGE(server_handle->list.head, server_handle->list.tail, "on two nodes tail should be different than head");
}

void test_if_cannot_register_two_cmds_with_the_same_base_and_cmd(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;
    char* test_cmd_base = "test_base";
    char* test_cmd = "test_cmd";
    int err = 0;

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    err = tcp_server_register_cmd(&server_handle, test_cmd_fun, test_cmd_base, test_cmd);

    if(err != 0) {
        TEST_FAIL_MESSAGE("first tcp_server_register_cmd returned err");
    }

    err = tcp_server_register_cmd(&server_handle, test_cmd_fun, test_cmd_base, test_cmd);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_TCPS_DUPLICATE_CMD, err, "trying to register two the same cmd should return ERR_TCPS_DUPLICATE_CMD");    
}

void test_if_can_register_two_cmds_with_the_same_base_and_different_cmd(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;
    char* test_cmd_base = "test_base";
    char* test_cmd = "test_cmd";
    char* different_cmd = "different_cmd";
    int err = 0;

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    err = tcp_server_register_cmd(&server_handle, test_cmd_fun, test_cmd_base, test_cmd);

    if(err != 0) {
        TEST_FAIL_MESSAGE("first tcp_server_register_cmd returned err");
    }

    err = tcp_server_register_cmd(&server_handle, test_cmd_fun, test_cmd_base, different_cmd);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "trying to register cmd with the same base but another cmd should not return error");    
}

void test_if_can_register_two_cmds_with_the_same_cmd_and_different_base(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;
    char* test_cmd_base = "test_base";
    char* test_cmd = "test_cmd";
    char* different_base = "different_cmd";
    int err = 0;

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    err = tcp_server_register_cmd(&server_handle, test_cmd_fun, test_cmd_base, test_cmd);

    if(err != 0) {
        TEST_FAIL_MESSAGE("first tcp_server_register_cmd returned err");
    }

    err = tcp_server_register_cmd(&server_handle, test_cmd_fun, different_base, test_cmd);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "trying to register cmd with the same cmd but another base should not return error");    
}

void test_if_the_same_chars_return_the_same_hash(void) {
    //given
    uint64_t first_hash = 0;
    uint64_t second_hash = 0;
    char* string_to_hash = "asadasdfgjytxfey4ytdasdasd";

    //when
    first_hash = tcp_server_hash_string(string_to_hash);
    second_hash = tcp_server_hash_string(string_to_hash);

    //then
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(first_hash, second_hash, "hash should always be the same on the same string");
}

void test_if_hash_string_fun_returns_err_on_null_string(void) {
    //given
    uint64_t err = 0;

    //when
    err = tcp_server_hash_string(NULL);

    //then
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(0, err, "tcp_server_hash_string should return 0 on null string");    
}

void test_if_find_hashed_cmd_finds_node(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;
    tcp_server_cmd_t* founded_node = NULL;
    char* base_to_find = "cmd_base";
    char* cmd_to_find = "cmd";
    uint64_t hbase = tcp_server_hash_string(base_to_find);
    uint64_t hcmd = tcp_server_hash_string(cmd_to_find);

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    tcp_server_register_cmd(&server_handle, test_cmd_fun, base_to_find, base_to_find);
    //tcp_server_register_cmd(&server_handle, test_cmd_fun, "dif_test_base", "dif_test_cmd");
    founded_node = tcp_server_find_string_cmd(&server_handle, base_to_find, cmd_to_find);

    //then
    TEST_ASSERT_NOT_NULL_MESSAGE(founded_node, "founded node should not be null");
}

void test_if_find_cmd_finds_node_with_correct_base(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;
    tcp_server_cmd_t* founded_node = NULL;
    char* base_to_find = "find_me_base!";
    char* cmd_to_find = "find_me_cmd!";
    uint64_t hbase = tcp_server_hash_string(base_to_find);
    uint64_t hcmd = tcp_server_hash_string(cmd_to_find);

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    tcp_server_register_cmd(&server_handle, test_cmd_fun, base_to_find, base_to_find);
    tcp_server_register_cmd(&server_handle, test_cmd_fun, "dif_test_base", "dif_test_cmd");
    founded_node = tcp_server_find_hashed_cmd(&server_handle, hbase, hcmd);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(hbase, founded_node->cmd_base, "cmd_base of founded node differs from expected");
}

void test_if_find_cmd_finds_node_with_correct_cmd(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;
    tcp_server_cmd_t* founded_node = NULL;
    char* base_to_find = "find_me_base!";
    char* cmd_to_find = "find_me_cmd!";
    uint64_t hbase = tcp_server_hash_string(base_to_find);
    uint64_t hcmd = tcp_server_hash_string(cmd_to_find);

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    tcp_server_register_cmd(&server_handle, test_cmd_fun, base_to_find, base_to_find);
    tcp_server_register_cmd(&server_handle, test_cmd_fun, "dif_test_base", "dif_test_cmd");
    founded_node = tcp_server_find_hashed_cmd(&server_handle, hbase, hcmd);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(hcmd, founded_node->cmd, "cmd of founded node differs from expected");
}

void test_if_find_cmd_finds_node_with_correct_fun(void) {
    //given
    tcp_server_handle_t* server_handle = NULL;
    tcp_server_cmd_t* founded_node = NULL;
    char* base_to_find = "find_me_base!";
    char* cmd_to_find = "find_me_cmd!";
    uint64_t hbase = tcp_server_hash_string(base_to_find);
    uint64_t hcmd = tcp_server_hash_string(cmd_to_find);

    //when
    tcp_server_init(&server_handle, TEST_PORT, TEST_ADDRESS);
    tcp_server_register_cmd(&server_handle, test_cmd_fun, base_to_find, base_to_find);
    tcp_server_register_cmd(&server_handle, test_other_fun, "dif_test_base", "dif_test_cmd");
    founded_node = tcp_server_find_hashed_cmd(&server_handle, hbase, hcmd);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(test_cmd_fun, founded_node->cmd_fun, "function of founded node differs from expected");
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
    RUN_TEST(test_if_head_is_not_null_after_adding_node);
    RUN_TEST(test_if_tail_is_not_null_after_adding_node);
    RUN_TEST(test_if_head_equals_tail_after_adding_one_node);
    RUN_TEST(test_if_register_cmd_returns_zero_on_correct_params);
    RUN_TEST(test_if_register_cmd_returns_err_on_null_handle);
    RUN_TEST(test_if_register_cmd_returns_err_on_null_cmd_base);
    RUN_TEST(test_if_register_cmd_returns_err_on_null_cmd);
    RUN_TEST(test_if_register_cmd_returns_err_on_null_function);
    RUN_TEST(test_if_hash_cmd_base_of_new_node_is_correct);
    RUN_TEST(test_if_hash_cmd_of_new_node_is_correct);
    RUN_TEST(test_if_fun_pointer_of_new_node_is_correct);
    RUN_TEST(test_if_head_is_different_than_head_on_two_registered_cmd);
    RUN_TEST(test_if_cannot_register_two_cmds_with_the_same_base_and_cmd);
    RUN_TEST(test_if_can_register_two_cmds_with_the_same_base_and_different_cmd);
    RUN_TEST(test_if_can_register_two_cmds_with_the_same_cmd_and_different_base);
    RUN_TEST(test_if_the_same_chars_return_the_same_hash);
    RUN_TEST(test_if_hash_string_fun_returns_err_on_null_string);
    return UNITY_END();
}