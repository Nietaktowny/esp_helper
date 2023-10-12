#include "unity.h"
#include "log.h"
#include "time.h"
#include "tcp_server.h"
#include "tcp_server_internal.h"
#include "error.h"

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

void test_if_addr_result_uses_af_inet(void) {
    //given
    tcp_server_handle_t* test_handle = NULL;
    int random_port = 27015;
    
    //when
    tcp_server_init(&test_handle, random_port, "127.0.0.1");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(AF_INET, test_handle->result->ai_family, "addr result ai_family should be AF_INET");

    //after
    tcp_server_deinit(&test_handle);
}

void test_if_addr_result_socktype_is_stream(void) {
    //given
    tcp_server_handle_t* test_handle = NULL;
    int random_port = 27015;
    
    //when
    tcp_server_init(&test_handle, random_port, "127.0.0.1");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(SOCK_STREAM, test_handle->result->ai_socktype, "addr result ai_socktype should be SOCK_STREAM");

    //after
    tcp_server_deinit(&test_handle);
}

void test_if_addr_result_protocol_is_tcp(void) {
    //given
    tcp_server_handle_t* test_handle = NULL;
    int random_port = 27015;
    
    //when
    tcp_server_init(&test_handle, random_port, "127.0.0.1");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(IPPROTO_TCP, test_handle->result->ai_protocol, "addr result ai_protocol should be IPPROTO_TCP");

    //after
    tcp_server_deinit(&test_handle);
}

void test_winsock_version_from_wsa_data(void) {
    //given
    tcp_server_handle_t* test_handle = NULL;
    int random_port = 27015;
    WORD expected_version = MAKEWORD(2,2);
    
    //when
    tcp_server_init(&test_handle, random_port, "127.0.0.1");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(expected_version, test_handle->wsaData.wVersion, "WSAData vVersion should be 2.2");

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
    int option_len = sizeof(int);
    
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
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
    SOCKET test_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // Binding to the already used socket
    int socket_option;
    int option_len = sizeof(int);
    
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

void test_if_winsock_was_initialized(void) {
    //given
    WSADATA wsaData;

    //when
    tcp_server_init_winsock(&wsaData);

    //then
    TEST_ASSERT_NOT_NULL_MESSAGE(&wsaData, "WSAData shouldn't be null after tcp_server_init_winsock");
}

void test_if_winsock_returns_error_multiple_inits(void) {
    //given
    WSADATA wsaData;
    int err;

    //when
    for (u_int i = 0; i < 4; i++)
    {
        err = tcp_server_init_winsock(&wsaData);
    }
    
    //then
    TEST_ASSERT_NOT_NULL_MESSAGE(&wsaData, "WSAData shouldn't be null after tcp_server_init_winsock");
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "tcp_server_init_winsock should not return err even on multiple calls");
}

void test_if_prepare_address_returns_zero(void) {
    //given
    int err = 0;
    struct addrinfo* result;
    const char* test_address = "127.46.77.12";

    //when
    err = tcp_server_prepare_address(&result, test_address);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "tcp_server_prepare_adress should return 0");
}

void test_if_prepare_address_uses_ipv4(void) {
    //given
    struct addrinfo* result;
    const char* test_address = "127.46.77.12";
    int family = 0;

    //when
    tcp_server_prepare_address(&result, test_address);

    //then
    family = result->ai_family;
    TEST_ASSERT_EQUAL_MESSAGE(AF_INET, family, "tcp_server_prepare_adress should use IPv4 addresses");
}

void test_if_prepare_address_uses_correct_sock_type(void) {
    //given
    struct addrinfo* result;
    const char* test_address = "127.46.77.12";
    int socktype = 0;

    //when
    tcp_server_prepare_address(&result, test_address);

    //then
    socktype = result->ai_socktype;
    TEST_ASSERT_EQUAL_MESSAGE(SOCK_STREAM, socktype, "tcp_server_prepare_adress should use stream sockets");
}

void test_if_prepare_address_uses_tcp_protocol(void) {
    //given
    struct addrinfo* result;
    const char* test_address = "12.43.7.1";
    int protocol = 0;

    //when
    tcp_server_prepare_address(&result, test_address);

    //then
    protocol = result->ai_protocol;
    TEST_ASSERT_EQUAL_MESSAGE(IPPROTO_TCP, protocol, "tcp_server_prepare_adress should use TCP");
}

void test_if_prepare_address_returns_err_on_too_short_address(void) {
    //given
    int err = 0;
    struct addrinfo* result;
    const char* test_address = "1.1.1";

    //when
    err = tcp_server_prepare_address(&result, test_address);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(WSAHOST_NOT_FOUND, err, "tcp_server_prepare_adress should return err when ip is wrong");
}

void test_if_prepare_address_returns_err_on_null_address(void) {
    //given
    int err = 0;
    struct addrinfo* result;
    const char* test_address = NULL;

    //when
    err = tcp_server_prepare_address(&result, test_address);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(ERR_NULL_POINTER, err, "tcp_server_prepare_adress should return err when address is null");
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_if_tcp_server_init_allocs_handler);
    RUN_TEST(test_if_tcp_server_handler_is_null_after_deinit);
    RUN_TEST(test_if_port_is_correct_after_tcp_init);
    RUN_TEST(test_if_tcp_server_init_returns_zero);
    RUN_TEST(test_if_addr_result_uses_af_inet);
    RUN_TEST(test_if_addr_result_socktype_is_stream);
    RUN_TEST(test_if_addr_result_protocol_is_tcp);
    RUN_TEST(test_winsock_version_from_wsa_data);
    RUN_TEST(test_if_listen_socket_is_not_invalid);
    RUN_TEST(test_if_listen_socket_has_reuse_option);
    RUN_TEST(test_if_set_reuse_addr_func_works);
    RUN_TEST(test_if_listen_socket_has_correct_ip);
    RUN_TEST(test_if_listen_socket_has_correct_port);
    RUN_TEST(test_if_winsock_was_initialized);
    RUN_TEST(test_if_winsock_returns_error_multiple_inits);
    RUN_TEST(test_if_prepare_address_returns_zero);
    RUN_TEST(test_if_prepare_address_uses_ipv4);
    RUN_TEST(test_if_prepare_address_uses_correct_sock_type);
    RUN_TEST(test_if_prepare_address_uses_tcp_protocol);
    RUN_TEST(test_if_prepare_address_returns_err_on_too_short_address);
    RUN_TEST(test_if_prepare_address_returns_err_on_null_address);

    return UNITY_END();
}