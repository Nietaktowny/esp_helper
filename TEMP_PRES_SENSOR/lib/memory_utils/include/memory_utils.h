#include <stdlib.h>
#include <inttypes.h>
#include "logger.h"
#include "arena.h"

/**
 * @brief Wrapper macro with malloc using type of new object.
 * 
 * @param object Identifer of object, where pointer from malloc will be stored.
 * @param type   Type of object, size of allocation is measured using sizeof(type).
 * 
 * 
 * @warning This macro calls exit() function when malloc fails.
*/
#define NEW(object, type)                   \
do {                                        \
  object = malloc(sizeof(type));            \
  if(!object)                               \
  {                                         \
    LOG_ERROR("error when trying to allocate memory"); \
    exit(EXIT_FAILURE);                     \
  }                                         \
} while (0)

/**
 * @brief Wrapper macro with malloc using size of object.
 * 
 * @param object Identifer of object, where pointer from malloc will be stored.
 * @param size   Size of allocation.
 * 
 * When malloc fails this logs error, and exits the program.
 * 
 * @warning This macro calls exit() function when malloc fails OR when size of allocation is 0.
*/
#define NEW_SIZE(object, size)              \
do {                                        \
  if(size == 0) {                           \
    LOG_ERROR("cannot allocate 0 sized memory");\
    exit(EXIT_FAILURE);                     \
  }                                         \
  object = malloc(size);                    \
  if(!object)                               \
  {                                         \
    LOG_ERROR("error when trying to allocate memory"); \
    exit(EXIT_FAILURE);                     \
  }                                         \
} while (0)


/**
 * @brief Wrapper macro for free.
 * 
 * @note It explicitly invalidates the pointer to freed memory, by setting it to NULL.
 * 
*/
#define DELETE(object)          \
do {                            \
  free(object);                 \
  object = NULL;                \
} while (0)                     


/**
 * @brief Fill memory with constant byte.
 * @note This should behave as C11 memset_s replacement.
 * 
 * @param void* destination
 * @param uint8_t byte with which destination memory fill be filled
 * @param size_t size of destination
*/
void* memutil_secure_memset(void* dest, uint8_t c, size_t n);

/**
 * @brief Zeroes memory.
 * 
 * @param void* destination of memory to be zeroed
 * @param size_t size of memory to be zeroed
 * 
 * @return 
 *          - dest if successfull
 *          - NULL on error
*/
void* memutil_zero_memory(void* dest, size_t size);

/**
 * @brief Changes uint64 value from host to network (Big endian) byte order.
*/
uint64_t memutil_htonll(uint64_t n);

/**
 * @brief Changes uint64 value from network (Big endian) to host byte order.
*/
uint64_t memutil_ntohll(uint64_t n);