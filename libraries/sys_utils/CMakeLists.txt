
idf_component_register(
    SRCS "src/sys_utils.c" 
    INCLUDE_DIRS "include"
    PRIV_REQUIRES logger err_controller memory_utils)