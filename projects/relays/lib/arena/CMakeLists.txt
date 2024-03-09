idf_component_register(
        SRCS "src/arena.c" 
        INCLUDE_DIRS "include" 
        PRIV_REQUIRES logger err_controller)