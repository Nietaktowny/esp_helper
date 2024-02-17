idf_component_register(
    SRCS "src/mysql_client.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES err_controller logger memory_utils)