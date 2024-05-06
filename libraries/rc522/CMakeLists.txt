
    idf_component_register(
        SRCS "src/rc522.c" 
        INCLUDE_DIRS "include"
        PRIV_REQUIRES logger err_controller memory_utils)