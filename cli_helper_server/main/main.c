#include "tcp_driver.h"
#include "tcp_driver_errors.h"
#include "logger.h"
#include "arena.h"
#include "err_controller.h"
#include "errors_list.h"
#include "memory_utils.h"
#include "linked_list.h"
#include "linked_list_errors.h"
#include "stdio.h"
#include <stdlib.h>
#include <unistd.h>
#include "cli_manager.h"

#include "assert.h"

int main(int argc, char **argv) {
	cli_create_logging_server("192.168.0.108", 27015);
    //LOG_ERROR("ERROR!");
	while (1)
	{
        LOG_INFO("Hello world!");
		sleep(2);
	}
	
	return 0;
}