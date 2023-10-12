add_executable(test_tcp_server test_tcp_server.c)
target_link_libraries(test_tcp_server PRIVATE tcp_server unity log error)
include_directories(${unity_SOURCE_DIR}/src)
include_directories(${tcp_server_SOURCE_DIR}/internals)


add_test(NAME test_tcp_server COMMAND test_tcp_server)
