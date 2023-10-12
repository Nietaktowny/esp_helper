add_executable(main main.c)

target_link_libraries(main PUBLIC log error tcp_client)