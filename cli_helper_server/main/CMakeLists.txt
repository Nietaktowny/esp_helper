add_executable(main main.c)

target_link_libraries(main PUBLIC tcp_server log error)