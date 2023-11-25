add_library(errors STATIC
        src/errors.c
)
target_include_directories(errors PUBLIC
        include
)

target_link_libraries(errors logger)

target_sources(errors PRIVATE src/errors.c)