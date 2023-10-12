add_library(tcp_client STATIC
        src/tcp_client.c
)
target_include_directories(tcp_client PUBLIC
        include
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(tcp_client PRIVATE log error ws2_32 Mswsock AdvApi32 Threads::Threads)

target_sources(tcp_client PRIVATE src/tcp_client.c)