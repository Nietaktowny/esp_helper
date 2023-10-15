cmake_minimum_required(VERSION 3.25)
project(cli_helper_client
        VERSION 0.1
        DESCRIPTION "TCP client for CLI Helper"
        LANGUAGES C)

set(CMAKE_C_STANDARD 11)

add_compile_options(-Wall -Wextra -Wpedantic)

enable_testing()

add_subdirectory(main)
add_subdirectory(test)
add_subdirectory(lib)
