add_library(tcp_driver STATIC
		src/tcp_driver.c
)

target_include_directories(tcp_driver PUBLIC
		include
)

target_link_libraries(tcp_driver logger memory_utils errors)

target_sources(tcp_driver PRIVATE src/tcp_driver.c)