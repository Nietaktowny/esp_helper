
#include "logger.h"


void app_main(void)
{
    //Init logger library, needs to be done before any logging.
    logger_init();

    //log verbose default color is white
    LOG_VERBOSE("Verbose log");

    //log debug, default color is white
    LOG_DEBUG("Debug log");

    //log info, default color is green
    LOG_INFO("Info log");

    //log warn, default color is yellow
    LOG_WARN("Warn log");

    //log error, default color is red
    LOG_ERROR("Error log");

    //log fatal error, default color is red bold underline
    LOG_FATAL("Fatal log");
}