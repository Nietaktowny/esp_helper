add_library(linked_list STATIC
        src/linked_list.c
)
target_include_directories(linked_list PUBLIC
        include
)

target_link_libraries(linked_list logger err_controller memory_utils)

target_sources(linked_list PRIVATE src/linked_list.c)