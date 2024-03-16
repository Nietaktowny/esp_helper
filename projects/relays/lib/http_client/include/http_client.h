/**
 * @file http_client.h
 * @author Wojciech Mytych (wojciech.luksz.mytych@gmail.com)
 * @brief HTTP Client header file.
 * @version 1.0.6
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stddef.h>

#define HTTP_CLIENT_MAX_OUTPUT_BUFFER_SIZE 				1024        ///< Maximum size for local response buffer.
#define HTTP_CLIENT_MAX_URL_LENGTH						256         ///< Maximum size for local URL buffer.
#define HTTP_CLIENT_POST_USE_STRLEN                     -1          ///< Use strlen to get post data len.

struct http_client_obj;
typedef struct http_client_obj* http_client_t;


/**
 * @brief Make a HTTP POST request.
 * 
 * @warning This function doesn't copy post_data, it's the responsibilty of user to keep it in scope.
 * 
 * @note Max URL size is set by HTTP_CLIENT_MAX_URL_LENGTH.
 * 
 * @note When HTTP_CLIENT_POST_USE_STRLEN is passed as datalen, post_data length is checked used strlen.
 * 
 * @param ipv4_address Domain address (without http://). 
 * @param path Path after the domain (something like /docs/get_address.php).
 * @param post_data Data to add as post.
 * @param datalen Post data length.
 * 
 * @retval ERR_C_NULL_POINTER If ipv4_address, path or post_data is NULL.
 * @retval ID of server response.
 * 
 */
int http_client_post(const char* ipv4_address, const char* path, const char* post_data, size_t datalen);


/**
 * @brief Make a HTTP POST request.
 * 
 * @note Max URL size is set by HTTP_CLIENT_MAX_URL_LENGTH.
 * 
 * @param ipv4_address Domain address (without http://). 
 * @param path Path after the domain (something like /docs/get_address.php).
 * @param buffer Buffer to store response.
 * @param buflen Buffer size.
 * 
 * @retval ERR_C_NULL_POINTER If ipv4_address, path or post_data is NULL.
 * @retval ERR_C_MEMORY_ERR If buffer is not big enough to store response.
 * @retval ID of server response.
 * 
 */
int http_client_get(const char* ipv4_address, const char* path, char* buffer, size_t buflen);

/**
 * @brief Make a HTTP GET request reusing the already made connection.
 * 
 * @param handle Http client handle, used to keep connection context.
 * @param buffer Buffer to store response.
 * @param buflen Buffer size.
 * 
 * @retval ERR_C_NULL_POINTER If handle or post_data is NULL.
 * @retval ERR_C_MEMORY_ERR If buffer is not big enough to store response.
 * @retval ID of server response.
 */
int http_client_get_reuse(http_client_t handle, char *buffer, size_t buflen);

/**
 * @brief Make a HTTP POST request the already made connection.
 * 
 * @param handle Http client handle, used to keep connection context.
 * @param post_data 
 * @param datalen 
 * @return int 
 */
int http_client_post_reuse(http_client_t handle ,const char *post_data, size_t datalen);

int http_client_init_reuse(http_client_t *handle, const char *ipv4_address, const char *path);

int http_client_deinit_reuse(http_client_t *handle);