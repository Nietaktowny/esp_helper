
    idf_component_register(
        SRCS "src/spi_controller.c" 
        INCLUDE_DIRS "include"
        PRIV_REQUIRES logger err_controller memory_utils)