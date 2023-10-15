#pragma once
#include "log.h"
#include <string.h>
#include <stdlib.h>

#define CHECK_FATAL_ERROR(err)	                                               \
  do {                                                                         \
    if (err != 0) {                                                            \
        LOG_FATAL("error code: %d\nerror name: %s", err, err_to_name(err));            \
        exit(1);                                                                \
    }                                                                          \
  } while (0)



#define CHECK_ERROR(err)                                                      \
  do {                                                                         \
    if (err != 0) {                                                         \
      LOG_ERROR("error code: %d\nerror name: %s", err, err_to_name(err));            \
    }                                                                          \
  } while (0)


#define CHECK_WSA_ERROR(err)                                                       \
  do {                                                                         \
    if (err != 0) {                                                          \
      LOG_ERROR("error code: %d", errno());              \
    }                                                                          \
  } while (0)


/*ERROR CODES*/
#define ERR_WINSOCK_INIT_ERROR       0x0001
#define ERR_NULL_POINTER  33001
#define ERR_NO_MEMORY     33002



/*ERROR NAMES*/
#define ERR_WINSOCK_INIT_ERROR_NAME "Error when initializing Winsock"


char *err_to_name(int err);
