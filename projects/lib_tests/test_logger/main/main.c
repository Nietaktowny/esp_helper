#include "logger.h"

void app_main(void)
{
    logger_init();
    logger_set_log_level(LOG_LEVEL_VERBOSE);
    
    LOG_VERBOSE("Verbose log")
    LOG_DEBUG("Debug log");
    LOG_INFO("Info log");
    LOG_WARN("Warn log");
    LOG_ERROR("Error log");
    LOG_FATAL("Fatal log");
}