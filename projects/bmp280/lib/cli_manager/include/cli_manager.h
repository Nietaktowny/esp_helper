#include "inttypes.h"

#define CLI_PORT                        27015
#define CLI_KEEPALIVE_IDLE              5               ///< TCP keep-alive idle time(s).
#define CLI_KEEPALIVE_INTERVAL          5               ///< TCP keep-alive interval time(s).
#define CLI_KEEPALIVE_COUNT             3               ///< TCP keep-alive packet retry send counts.
#define CLI_CMD_INTS_NUMBER             2               ///< Number of ints used to indicate command in received data.
#define CLI_CMD_ARGS_NUMBER             64              ///< Number of chars used to indicate arguments in received data.
#define CLI_CMD_MAX_COMMANDS            64              ///< Max number of commands to register.

#define CLI_ERR_BASE                    0x0500
#define CLI_ERR_CONNECT_FAIL            CLI_ERR_BASE + 0x0001


int cli_set_remote_logging(uint16_t port);

void cli_accept_task(void* args);