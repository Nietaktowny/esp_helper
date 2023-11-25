add_library(memory_utils STATIC
        src/memory_utils.c
)
target_include_directories(memory_utils PUBLIC
        include
)

target_link_libraries(memory_utils logger errors)

target_sources(memory_utils PRIVATE src/memory_utils.c)