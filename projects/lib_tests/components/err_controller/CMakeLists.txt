idf_component_register(
    SRCS "src/err_controller.c" 
    INCLUDE_DIRS "include" 
    PRIV_REQUIRES CException logger)