add_executable(test_tcp_client test_tcp_client.c)
target_link_libraries(test_tcp_client PRIVATE tcp_client unity log error)
include_directories(${unity_SOURCE_DIR}/src)
include_directories(${tcp_server_SOURCE_DIR}/internals)


add_test(NAME test_tcp_client COMMAND test_tcp_client)
