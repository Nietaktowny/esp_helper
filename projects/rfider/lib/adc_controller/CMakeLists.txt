
    idf_component_register(
        SRCS "src/adc_controller.c" 
        INCLUDE_DIRS "include"
        PRIV_REQUIRES esp_adc logger err_controller memory_utils)
