#include <stddef.h>



int http_client_post(const char* ipv4_address, const char* path, const char* post_data);

int https_client_post(const char* ipv4_address, const char* path, const char* post_data);

int http_client_get(const char* ipv4_address, const char* path, char* buffer, size_t buflen);