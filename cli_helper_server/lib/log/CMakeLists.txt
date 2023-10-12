add_library(log STATIC
        src/log.c
)
target_include_directories(log PUBLIC
        include
)

target_sources(log PRIVATE src/log.c)