#include "logger.h"
#include <stdio.h>

void app_main(void)
{
    //Init logger library, needs to be done before any logging.
    logger_init();

    //Set log level to verbose, to make verbose logs visible
    logger_set_log_level(LOG_LEVEL_VERBOSE);

    //Open file for logging
    FILE* file = fopen("logfile.txt", "w+");
    
    //add log file to logger
    logger_add_log_file(file);

    //Silent mode is supported, in which logs are logged only to log files, but not to stderr
    logger_enable_silent_mode();

    LOG_VERBOSE("Verbose log");


    LOG_DEBUG("Debug log");


    LOG_INFO("Info log");


    LOG_WARN("Warn log");


    LOG_ERROR("Error log");


    LOG_FATAL("Fatal log");
}