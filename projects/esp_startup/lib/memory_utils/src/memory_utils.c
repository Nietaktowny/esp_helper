#include "memory_utils.h"
#include "logger.h"
#include <arpa/inet.h>

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

uint64_t memutil_htonll(uint64_t n) {
    #if __BYTE_ORDER == __BIG_ENDIAN
        return n; 
    #else
        return (((uint64_t)htonl(n)) << 32) + htonl(n >> 32);
    #endif
}

uint64_t memutil_ntohll(uint64_t n) {
    #if __BYTE_ORDER == __BIG_ENDIAN
        return n;
    #else
        return (((uint64_t)ntohl(n)) << 32) + ntohl(n >> 32);
    #endif
}