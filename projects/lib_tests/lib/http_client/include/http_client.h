/**
 * @file http_client.h
 * @author Wojciech Mytych (wojciech.luksz.mytych@gmail.com)
 * @brief HTTP Client header file.
 * @version 1.0.5
 * @date 2024-02-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stddef.h>

#define HTTP_CLIENT_MAX_OUTPUT_BUFFER_SIZE 				1024        ///< Maximum size for local response buffer.
#define HTTP_CLIENT_MAX_URL_LENGTH						256         ///< Maximum size for local URL buffer.
#define HTTP_CLIENT_POST_USE_STRLEN                     -1          ///< Use strlen to get post data len.

/**
 * @brief Make a HTTP POST request.
 * 
 * @warning This function doesn't copy post_data, it's the responsibilty of user to keep it in scope.
 * 
 * @attention Max URL size is set by HTTP_CLIENT_MAX_URL_LENGTH.
 * 
 * @note When HTTP_CLIENT_POST_USE_STRLEN is passed as datalen, post_data length is checked used strlen.
 * 
 * @param ipv4_address Domain address (without http://). 
 * @param path Path after the domain (something like /docs/get_address.php).
 * @param post_data Data to add as post.
 * @param datalen Post data length.
 * 
 * @retval ERR_NULL_POINTER If ipv4_address, path or post_data is NULL.
 * @retval ID of server response.
 * 
 */
int http_client_post(const char* ipv4_address, const char* path, const char* post_data, size_t datalen);


/**
 * @brief Make a HTTP POST request.
 * 
 * @attention Max URL size is set by HTTP_CLIENT_MAX_URL_LENGTH.
 * 
 * @param ipv4_address Domain address (without http://). 
 * @param path Path after the domain (something like /docs/get_address.php).
 * @param buffer Buffer to store response.
 * @param buflen Buffer size.
 * 
 * @retval ERR_NULL_POINTER If ipv4_address, path or post_data is NULL.
 * @retval ERR_C_MEMORY_ERR If buffer is not big enough to store response.
 * @retval ID of server response.
 * 
 */
int http_client_get(const char* ipv4_address, const char* path, char* buffer, size_t buflen);