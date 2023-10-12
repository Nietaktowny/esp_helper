add_library(error STATIC
        src/error.c
)
target_include_directories(error PUBLIC
        include
)

target_link_libraries(error log)

target_sources(error PRIVATE src/error.c)