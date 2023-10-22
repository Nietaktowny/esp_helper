add_executable(test_arena test_arena.c)
target_link_libraries(test_arena PRIVATE arena unity log)
include_directories(${unity_SOURCE_DIR}/src)
include_directories(${tcp_server_SOURCE_DIR}/internals)


add_test(NAME test_arena COMMAND test_arena)
