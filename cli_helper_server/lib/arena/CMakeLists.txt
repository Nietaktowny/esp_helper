add_library(arena STATIC
        src/arena.c
)
target_include_directories(arena PUBLIC
        include
)

target_link_libraries(arena log error)

target_sources(arena PRIVATE src/arena.c)