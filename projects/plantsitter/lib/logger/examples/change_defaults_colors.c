#include "ansi_colors.h"

// Logger library uses ANSI escape sequences to color logs in terminal
// Basic ANSI colors are defined in ansi_colors.h header used by logger library.

#define VERBOSE_COLOR   "\e[30;2;3m"            //use own not predefined ANSI escape sequence - faint + black color
#define DEBUG_COLOR     BLUE_BOLD               //bold + colors
#define INFO_COLOR      GREEN_ITALIC            //italic + colors
#define WARN_COLOR      YELLOW_BOLD_UNDERLINE   //bold + underline + colors
#define ERROR_COLOR     RED_BOLD_HIGH_INTENSITY // bold + high intensity + colors
#define FATAL_COLOR     PURPLE_BACKGROUND       // background color

#include "logger.h"


void app_main(void)
{
    //Init logger library, needs to be done before any logging.
    logger_init();

    //Set log level to verbose, to make verbose logs visible
    logger_set_log_level(LOG_LEVEL_VERBOSE);


    LOG_VERBOSE("Verbose log");


    LOG_DEBUG("Debug log");


    LOG_INFO("Info log");


    LOG_WARN("Warn log");


    LOG_ERROR("Error log");


    LOG_FATAL("Fatal log");
}