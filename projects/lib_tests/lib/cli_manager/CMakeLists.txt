idf_component_register(
    SRCS "src/cli_manager.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES tcp_controller wifi_controller err_controller logger memory_utils)