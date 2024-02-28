idf_component_register(
    SRCS "src/err_controller.c" "src/err_c_names.c"
    INCLUDE_DIRS "include" 
    PRIV_REQUIRES logger
    REQUIRES CException)