#include "memory_utils.h"
#include "logger.h"
#include <byteswap.h>

void* memutil_secure_memset(void* dest, uint8_t c, size_t n) {
    if(!dest) {
        LOG_ERROR("destination cannot be NULL");
        return NULL;
    }
    if(n == 0) {
        LOG_ERROR("size cannot be 0");
        return NULL;
    }

    volatile uint8_t* p = dest;
    while (n--)
        *p++ = (uint8_t)c;
    return dest;
}


void* memutil_zero_memory(void* dest, size_t size) {
    return memutil_secure_memset(dest, 0, size);
}

/**
 * @brief Convert host byte order to network byte order.
*/
uint64_t memutil_htonll(uint64_t x) {
#if BYTE_ORDER == BIG_ENDIAN
    return x;
#elif BYTE_ORDER == LITTLE_ENDIAN
    return __bswap64(x);
#else
#error "What kind of system is this?"
#endif
}

/**
 * @brief Convert host byte order to network byte order.
*/
uint32_t memutil_htonl(uint32_t x) {
#if BYTE_ORDER == BIG_ENDIAN
  return x;
#elif BYTE_ORDER == LITTLE_ENDIAN
  return __bswap_32(x);
#else
# error "What kind of system is this?"
#endif
}

/**
 * @brief Convert host byte order to network byte order.
*/
uint16_t memutil_htons(uint16_t x) {
#if BYTE_ORDER == BIG_ENDIAN
  return x;
#elif BYTE_ORDER == LITTLE_ENDIAN
  return __bswap_16 (x);
#else
# error "What kind of system is this?"
#endif
}

/**
 * @brief Convert network byte order to host byte order.
*/
uint64_t memutil_ntohll(uint64_t x) {
#if BYTE_ORDER == BIG_ENDIAN
    return x;
#elif BYTE_ORDER == LITTLE_ENDIAN
    return __bswap64(x);
#else
#error "What kind of system is this?"
#endif
}

/**
 * @brief Convert network byte order to host byte order.
*/
uint32_t memutil_ntohl(uint32_t x) {
#if BYTE_ORDER == BIG_ENDIAN
  return x;
#elif BYTE_ORDER == LITTLE_ENDIAN
  return __bswap_32(x);
#else
# error "What kind of system is this?"
#endif
}

/**
 * @brief Convert network byte order to host byte order.
*/
uint16_t memutil_ntohs(uint16_t x) {
#if BYTE_ORDER == BIG_ENDIAN
  return x;
#elif BYTE_ORDER == LITTLE_ENDIAN
  return __bswap_16 (x);
#else
# error "What kind of system is this?"
#endif
}