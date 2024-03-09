idf_component_register(
    SRCS "src/linked_list.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES err_controller logger memory_utils)
