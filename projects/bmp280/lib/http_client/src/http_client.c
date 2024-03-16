/**
 * @file http_client.c
 * @author Wojciech Mytych (wojciech.luksz.mytych@gmail.com)
 * @brief Http client source file.
 *
 * @todo Implement get and post versions using arena
 * @todo Implement ESP8266-RTOS-SDK compliant version
 *
 * @version 1.0.7
 * @date 2024-03-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "http_client.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_http_client.h"
#include "esp_tls.h"
#include "cJSON.h"
#include "esp_crt_bundle.h"

#include "err_controller.h"
#include "memory_utils.h"
#include "arena.h"
#include "logger.h"
#include "inttypes.h"

/**
 * @brief Http client context structure.
 *
 * Needed by persistent connections.
 *
 */
struct http_client_obj
{
	esp_http_client_config_t esp_cfg; 			/**!< ESP HTTP Client config.*/
	esp_http_client_handle_t esp_handle;		/**!< ESP HTTP Client handle.*/
	char url[HTTP_CLIENT_MAX_URL_LENGTH];		/**!< URL to make connection with.*/
};

/**
 * @brief HTTP Client event handler, called each time new HTTP event happens.
 *
 * @param evt  HTTP Client event.
 *
 * @retval ESP_FAIL if error occurs.
 * @retval ESP_OK if event is processed successfully.
 */
static esp_err_t _http_event_handler(esp_http_client_event_t *evt);

static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
	char *output_buffer = NULL; // Buffer to store response of http request from event handler
	int output_len = 0;			// Stores number of bytes read
	switch (evt->event_id)
	{
	case HTTP_EVENT_ERROR:
		LOG_VERBOSE("HTTP_EVENT_ERROR");
		break;
	case HTTP_EVENT_ON_CONNECTED:
		LOG_VERBOSE("HTTP_EVENT_ON_CONNECTED");
		break;
	case HTTP_EVENT_HEADER_SENT:
		LOG_VERBOSE("HTTP_EVENT_HEADER_SENT");
		break;
	case HTTP_EVENT_ON_HEADER:
		LOG_VERBOSE("HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
		break;
	case HTTP_EVENT_ON_DATA:
		LOG_VERBOSE("HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
		LOG_VERBOSE("HTTP_EVENT_ON_DATA, output_len=%d", output_len);
		LOG_VERBOSE("HTTP_EVENT_ON_DATA, content_length=%d", esp_http_client_get_content_length(evt->client));
		// If user_data buffer is configured, copy the response into the buffer
		if (evt->user_data)
		{
			memcpy(evt->user_data + output_len, evt->data, evt->data_len);
		}
		else
		{
			if (output_buffer == NULL && esp_http_client_get_content_length(evt->client) > 0)
			{
				output_buffer = (char *)malloc(esp_http_client_get_content_length(evt->client));
				output_len = 0;
				if (output_buffer == NULL)
				{
					LOG_ERROR("Failed to allocate memory for output buffer");
					return ESP_FAIL;
				}
			}
			memcpy(output_buffer + output_len, evt->data, evt->data_len);
		}
		output_len += evt->data_len;
		break;
	case HTTP_EVENT_ON_FINISH:
		LOG_VERBOSE("HTTP_EVENT_ON_FINISH");
		if (output_buffer != NULL)
		{
			// Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
			// ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
			free(output_buffer);
			output_buffer = NULL;
		}
		output_len = 0;
		break;
	case HTTP_EVENT_DISCONNECTED:
		LOG_VERBOSE("HTTP_EVENT_DISCONNECTED");
		int mbedtls_err = 0;
		esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
		if (err != 0)
		{
			if (output_buffer != NULL)
			{
				free(output_buffer);
				output_buffer = NULL;
			}
			output_len = 0;
			LOG_ERROR("Last esp error code: 0x%x", err);
			LOG_ERROR("Last mbedtls failure: 0x%x", mbedtls_err);
		}
		break;
	case HTTP_EVENT_REDIRECT:
		LOG_VERBOSE("HTTP_EVENT_REDIRECT");
		break;
	default:
		LOG_VERBOSE("NOT KNOWN HTTP EVENT");
		break;
	}
	return ESP_OK;
}

int http_client_post(const char *ipv4_address, const char *path, const char *post_data, size_t datalen)
{
	err_c_t err = 0;
	uint64_t id = 0;
	char local_response_buffer[HTTP_CLIENT_MAX_OUTPUT_BUFFER_SIZE] = {0};
	char url[HTTP_CLIENT_MAX_URL_LENGTH] = {0};

	ERR_C_CHECK_NULL_PTR(ipv4_address, LOG_ERROR("IPv4 address cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(path, LOG_ERROR("Path to php script cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(post_data, LOG_ERROR("Post data cannot be NULL."));

	snprintf(url, sizeof(url), "http://%s/%s", ipv4_address, path);
	LOG_INFO("making POST request to address: %s", url);
	// prepare client handle
	esp_http_client_config_t config = {
		.url = url,
		.event_handler = _http_event_handler,
		.user_data = local_response_buffer,
		.method = HTTP_METHOD_POST,
	};
	esp_http_client_handle_t client = esp_http_client_init(&config);

	// POST
	esp_http_client_set_url(client, url);
	esp_http_client_set_method(client, HTTP_METHOD_POST);
	// If datalen is set to -1, use strlen to determine post data size.
	if (datalen == HTTP_CLIENT_POST_USE_STRLEN)
	{
		esp_http_client_set_post_field(client, post_data, strlen(post_data));
	}
	else
	{
		esp_http_client_set_post_field(client, post_data, datalen);
	}
	err = esp_http_client_perform(client);
	if (err == ERR_C_OK)
	{
		LOG_DEBUG("HTTP GET Status = %d, content_length = %d",
				  esp_http_client_get_status_code(client),
				  esp_http_client_get_content_length(client));
		LOG_INFO("\n%s", local_response_buffer);
		id = strtol(local_response_buffer, NULL, 10);
	}
	else
	{
		LOG_ERROR("HTTP POST request failed: %s", esp_err_to_name(err));
	}
	esp_http_client_cleanup(client);
	return (int)id;
}

/**
 *
 * @todo Do error checking if provided buffer fits into internal url buffer.
 */
int http_client_get(const char *ipv4_address, const char *path, char *buffer, size_t buflen)
{
	err_c_t err = 0;
	uint64_t id = 0;
	char local_response_buffer[HTTP_CLIENT_MAX_OUTPUT_BUFFER_SIZE] = {0};
	char url[HTTP_CLIENT_MAX_URL_LENGTH] = {0};

	ERR_C_CHECK_NULL_PTR(ipv4_address, LOG_ERROR("IPv4 address cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(path, LOG_ERROR("Path to php script cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("Buffer to store http get result cannot be NULL"));

	memutil_zero_memory(buffer, buflen);

	snprintf(url, sizeof(url), "http://%s/%s%c", ipv4_address, path, '\0');
	LOG_DEBUG("making GET request to address: %s", url);
	// prepare client handle
	esp_http_client_config_t config = {
		.url = url,
		.event_handler = _http_event_handler,
		.user_data = local_response_buffer,
		.method = HTTP_METHOD_GET,
	};
	esp_http_client_handle_t client = esp_http_client_init(&config);

	// POST
	esp_http_client_set_url(client, url);
	esp_http_client_set_method(client, HTTP_METHOD_GET);
	err = esp_http_client_perform(client);
	if (err == ERR_C_OK)
	{
		LOG_DEBUG("HTTP GET Status = %d, content_length = %d",
				  esp_http_client_get_status_code(client),
				  esp_http_client_get_content_length(client));
		LOG_DEBUG("\n%s", local_response_buffer);
		id = strtol(local_response_buffer, NULL, 10);

		// copy result into user buffer
		size_t resplen = strlen(local_response_buffer);
		if (resplen >= buflen)
		{
			LOG_ERROR("buffer not big enough to store HTTP response:\nbuffer: %ld, content_length: %d", buflen, esp_http_client_get_content_length(client));
			return ERR_C_MEMORY_ERR;
		}

		memutil_zero_memory(buffer, buflen);
		mempcpy(buffer, local_response_buffer, resplen);
		LOG_VERBOSE("HTTP response stored into buffer");
	}
	else
	{
		LOG_ERROR("HTTP GET request failed: %s", esp_err_to_name(err));
	}
	esp_http_client_cleanup(client);
	return (int)id;
}

int http_client_init_reuse(http_client_t *handle, const char *ipv4_address, const char *path)
{
	err_c_t err = 0;

	ERR_C_CHECK_NULL_PTR(ipv4_address, LOG_ERROR("IPv4 address cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(path, LOG_ERROR("Path to php script cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("Location to store http_client handle cannot be NULL"));

	NEW(*handle, struct http_client_obj);

	snprintf((*handle)->url, HTTP_CLIENT_MAX_URL_LENGTH, "http://%s/%s", ipv4_address, path);
	LOG_VERBOSE("preparing http client handle for address: %s", (*handle)->url);

	// prepare client handle
	esp_http_client_config_t config = {
		.url = (*handle)->url,
		.event_handler = _http_event_handler,
		.method = HTTP_METHOD_GET,
		.keep_alive_enable = true,
		.keep_alive_interval = 5000,
		.keep_alive_count = 5,
		.keep_alive_idle = 10000,
		.timeout_ms = 10000,
	};

	(*handle)->esp_handle = esp_http_client_init(&config);

	esp_http_client_set_url((*handle)->esp_handle, (*handle)->url);

	LOG_DEBUG("http client handle prepared for reusing with multiple requests for url: %s", (*handle)->url);

	return err;
}

int http_client_deinit_reuse(http_client_t *handle)
{
	err_c_t err = 0;

	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("Location to store http_client handle cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(*handle, LOG_ERROR("http_client handle must be initialized."));

	// free esp specific resources
	esp_http_client_cleanup((*handle)->esp_handle);

	memutil_zero_memory((*handle), sizeof(struct http_client_obj));

	// free http client handle
	DELETE((*handle));

	LOG_DEBUG("http client handle deinit");

	return err;
}

/**
 *
 * @todo Do error checking if provided buffer fits into internal url buffer.
 */
int http_client_get_reuse(http_client_t handle, char *buffer, size_t buflen)
{
	err_c_t err = 0;
	//uint64_t id = 0;
	char local_response_buffer[HTTP_CLIENT_MAX_OUTPUT_BUFFER_SIZE] = {0};

	ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("Buffer to store http get result cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("http client handle cannot be NULL"));

	LOG_VERBOSE("making GET request to address: %s", handle->url);

	ERR_C_CHECK_ERROR(
		esp_http_client_set_method(handle->esp_handle, HTTP_METHOD_GET),
		LOG_ERROR("error %d when setting HTTP GET method in http client handle: %s", err, error_to_name(err)));
	
	ERR_C_CHECK_ERROR(
		esp_http_client_set_user_data(handle->esp_handle, local_response_buffer),
		LOG_ERROR("error %d when setting user data in http client handle: %s", err, error_to_name(err)));

	err = esp_http_client_perform(handle->esp_handle);
	if (err == ERR_C_OK)
	{
		LOG_DEBUG("HTTP GET Status = %d, content_length = %d",
				  esp_http_client_get_status_code(handle->esp_handle),
				  esp_http_client_get_content_length(handle->esp_handle));
		LOG_DEBUG("\n%s", local_response_buffer);
		//id = strtol(local_response_buffer, NULL, 10);

		// copy result into user buffer
		size_t resplen = strlen(local_response_buffer);
		if (resplen >= buflen)
		{
			LOG_ERROR("buffer not big enough to store HTTP response:\nbuffer: %ld, content_length: %d", buflen, esp_http_client_get_content_length(handle->esp_handle));
			return ERR_C_MEMORY_ERR;
		}

		memutil_zero_memory(buffer, buflen);
		mempcpy(buffer, local_response_buffer, resplen);
		LOG_VERBOSE("HTTP response stored into buffer");
	}
	else
	{
		LOG_ERROR("HTTP GET request failed: %s", esp_err_to_name(err));
		LOG_WARN("Trying to reopen the connection...");
		ERR_C_CHECK_ERROR(esp_http_client_close(handle->esp_handle), LOG_ERROR("Closing old connection failed."));
		ERR_C_CHECK_ERROR(esp_http_client_open(handle->esp_handle, 0), LOG_ERROR("Trying to open new connection failed."));
	}
	return err;
}

/**
 *
 * @todo Do error checking if provided buffer fits into internal url buffer.
 */
int http_client_post_reuse(http_client_t handle, const char *post_data, size_t datalen)
{
	err_c_t err = 0;
	char local_response_buffer[HTTP_CLIENT_MAX_OUTPUT_BUFFER_SIZE] = {0};

	ERR_C_CHECK_NULL_PTR(post_data, LOG_ERROR("Post data cannot be NULL."));
	ERR_C_CHECK_NULL_PTR(handle, LOG_ERROR("http client handle cannot be NULL"));

	LOG_VERBOSE("making POST request to address: %s", handle->url);

	// POST
	ERR_C_CHECK_ERROR(
		esp_http_client_set_method(handle->esp_handle, HTTP_METHOD_POST),
		LOG_ERROR("error %d when setting HTTP POST method in http client handle: %s", err, error_to_name(err)));
	
	ERR_C_CHECK_ERROR(
		esp_http_client_set_user_data(handle->esp_handle, local_response_buffer),
		LOG_ERROR("error %d when setting user data in http client handle: %s", err, error_to_name(err)));

	// If datalen is set to -1, use strlen to determine post data size.
	if (datalen == HTTP_CLIENT_POST_USE_STRLEN)
	{
		esp_http_client_set_post_field(handle->esp_handle, post_data, strlen(post_data));
	}
	else
	{
		esp_http_client_set_post_field(handle->esp_handle, post_data, datalen);
	}
	err = esp_http_client_perform(handle->esp_handle);
	if (err == ERR_C_OK)
	{
		LOG_DEBUG("HTTP GET Status = %d, content_length = %d",
				  esp_http_client_get_status_code(handle->esp_handle),
				  esp_http_client_get_content_length(handle->esp_handle));
		LOG_INFO("\n%s", local_response_buffer);
	}
	else
	{
		LOG_ERROR("HTTP POST request failed: %s", esp_err_to_name(err));
	}

	return err;
}
