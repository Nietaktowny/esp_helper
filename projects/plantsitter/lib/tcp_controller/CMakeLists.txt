idf_component_register(
    SRCS "src/tcp_controller.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES err_controller logger memory_utils)
