add_library(err_controller STATIC
        src/err_controller.c
)
target_include_directories(err_controller PUBLIC
        include
)

target_link_libraries(err_controller logger cexception)

target_sources(err_controller PRIVATE src/err_controller.c)