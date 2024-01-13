add_library(cli_manager STATIC
        src/cli_manager.c
)
target_include_directories(cli_manager PUBLIC
        include
)

target_link_libraries(cli_manager logger tcp_driver memory_utils linked_list err_controller arena cexception)

target_sources(cli_manager PRIVATE src/cli_manager.c)