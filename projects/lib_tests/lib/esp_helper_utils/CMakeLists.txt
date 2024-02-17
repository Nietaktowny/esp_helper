idf_component_register(
    SRCS "src/esp_helper_utils.c" 
    INCLUDE_DIRS "include"
    PRIV_REQUIRES sys_utils logger err_controller memory_utils)