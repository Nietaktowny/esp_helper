add_library(arena STATIC
        src/arena.c
)
target_include_directories(arena PUBLIC
        include
)

target_link_libraries(arena logger errors)

target_sources(arena PRIVATE src/arena.c)