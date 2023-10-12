add_library(tcp_server STATIC
        src/tcp_server.c
)
target_include_directories(tcp_server
        PUBLIC
                include
                internals
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(tcp_server PRIVATE log error ws2_32 Mswsock AdvApi32 Threads::Threads)

target_sources(tcp_server PRIVATE src/tcp_server.c)