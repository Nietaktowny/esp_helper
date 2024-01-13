add_executable(main main.c)

target_link_libraries(main PUBLIC tcp_driver logger err_controller linked_list memory_utils arena cli_manager)