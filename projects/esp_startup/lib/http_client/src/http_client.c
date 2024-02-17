#include "http_client.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_http_client.h"
#include "esp_tls.h"
#include "cJSON.h"
#include "esp_crt_bundle.h"

#include "err_controller.h"
#include "errors_list.h"
#include "memory_utils.h"
#include "arena.h"
#include "logger.h"
#include "inttypes.h"

#define MAX_HTTP_OUTPUT_BUFFER 2048

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
	static char *output_buffer;  // Buffer to store response of http request from event handler
	static int output_len;		 // Stores number of bytes read
	switch(evt->event_id) {
		case HTTP_EVENT_ERROR:
			LOG_DEBUG("HTTP_EVENT_ERROR");
			break;
		case HTTP_EVENT_ON_CONNECTED:
			LOG_DEBUG("HTTP_EVENT_ON_CONNECTED");
			break;
		case HTTP_EVENT_HEADER_SENT:
			LOG_DEBUG("HTTP_EVENT_HEADER_SENT");
			break;
		case HTTP_EVENT_ON_HEADER:
			LOG_DEBUG("HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
			break;
		case HTTP_EVENT_ON_DATA:
			LOG_DEBUG("HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
			LOG_DEBUG("HTTP_EVENT_ON_DATA, output_len=%d", output_len);
			LOG_DEBUG("HTTP_EVENT_ON_DATA, content_length=%d", esp_http_client_get_content_length(evt->client));
			// If user_data buffer is configured, copy the response into the buffer
			if (evt->user_data) {
				memcpy(evt->user_data + output_len, evt->data, evt->data_len);
			} else {
				if (output_buffer == NULL && esp_http_client_get_content_length(evt->client) > 0) {
					output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
					output_len = 0;
					if (output_buffer == NULL) {
						LOG_ERROR("Failed to allocate memory for output buffer");
						return ESP_FAIL;
					}
				}
				memcpy(output_buffer + output_len, evt->data, evt->data_len);
			}
			output_len += evt->data_len;
			break;
		case HTTP_EVENT_ON_FINISH:
			LOG_DEBUG("HTTP_EVENT_ON_FINISH");
			if (output_buffer != NULL) {
				// Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
				// ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
				free(output_buffer);
				output_buffer = NULL;
			}
			output_len = 0;
			break;
		case HTTP_EVENT_DISCONNECTED:
			LOG_DEBUG("HTTP_EVENT_DISCONNECTED");
			int mbedtls_err = 0;
			esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
			if (err != 0) {
				if (output_buffer != NULL) {
					free(output_buffer);
					output_buffer = NULL;
				}
				output_len = 0;
				LOG_ERROR("Last esp error code: 0x%x", err);
				LOG_ERROR("Last mbedtls failure: 0x%x", mbedtls_err);
			}
			break;
        case HTTP_EVENT_REDIRECT:
            LOG_DEBUG("HTTP_EVENT_REDIRECT");
            break;
        default:
            LOG_DEBUG("NOT KNOWN HTTP EVENT");
            break;
	}
	return ESP_OK;
}

int https_client_post(const char* ipv4_address, const char* path, const char* post_data) {
    err_c_t err = 0;
    uint64_t id = 0;
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};
    char url[64];

    memutil_zero_memory(&url, sizeof(url));
    memutil_zero_memory(&local_response_buffer, sizeof(local_response_buffer));

    ERR_C_CHECK_NULL_PTR(ipv4_address, LOG_ERROR("IPv4 address cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(path, LOG_ERROR("Path to php script cannot be NULL"));

    sprintf(url, "http://%s/%s", ipv4_address, path);
    LOG_INFO("making POST request to address: %s", url);
    //prepare client handle
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_event_handler,
		.user_data = local_response_buffer,
		.transport_type = HTTP_TRANSPORT_OVER_SSL,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    //POST
    esp_http_client_set_url(client, url);
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    err = esp_http_client_perform(client);
    if(err == ERR_C_OK) {
 		LOG_DEBUG("HTTP GET Status = %d, content_length = %d",
				esp_http_client_get_status_code(client),
				esp_http_client_get_content_length(client));
		LOG_INFO("\n%s", local_response_buffer);
		id = strtol(local_response_buffer, NULL, 10);       
    } else {
        LOG_ERROR("HTTP POST request failed: %s", esp_err_to_name(err));
    }
	esp_http_client_cleanup(client);
    return (int)id;
}

int http_client_post(const char* ipv4_address, const char* path, const char* post_data) {
    err_c_t err = 0;
    uint64_t id = 0;
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};
    char url[64];

    memutil_zero_memory(&url, sizeof(url));
    memutil_zero_memory(&local_response_buffer, sizeof(local_response_buffer));

    ERR_C_CHECK_NULL_PTR(ipv4_address, LOG_ERROR("IPv4 address cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(path, LOG_ERROR("Path to php script cannot be NULL"));

    sprintf(url, "http://%s/%s", ipv4_address, path);
    LOG_INFO("making POST request to address: %s", url);
    //prepare client handle
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,
		.method = HTTP_METHOD_POST,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    //POST
    esp_http_client_set_url(client, url);
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    err = esp_http_client_perform(client);
    if(err == ERR_C_OK) {
 		LOG_DEBUG("HTTP GET Status = %d, content_length = %d",
				esp_http_client_get_status_code(client),
				esp_http_client_get_content_length(client));
		LOG_INFO("\n%s", local_response_buffer);
		id = strtol(local_response_buffer, NULL, 10);       
    } else {
        LOG_ERROR("HTTP POST request failed: %s", esp_err_to_name(err));
    }
	esp_http_client_cleanup(client);
    return (int)id;
}

int http_client_get(const char* ipv4_address, const char* path, char* buffer, size_t buflen) {
    err_c_t err = 0;
    uint64_t id = 0;
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};
    char url[64];

    memutil_zero_memory(&url, sizeof(url));
    memutil_zero_memory(&local_response_buffer, sizeof(local_response_buffer));

    ERR_C_CHECK_NULL_PTR(ipv4_address, LOG_ERROR("IPv4 address cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(path, LOG_ERROR("Path to php script cannot be NULL"));
	ERR_C_CHECK_NULL_PTR(buffer, LOG_ERROR("Buffer to store http get result cannot be NULL"));

    sprintf(url, "http://%s/%s", ipv4_address, path);
    LOG_INFO("making GET request to address: %s", url);
    //prepare client handle
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,
		.method = HTTP_METHOD_GET,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    //POST
    esp_http_client_set_url(client, url);
    esp_http_client_set_method(client, HTTP_METHOD_GET);
    err = esp_http_client_perform(client);
    if(err == ERR_C_OK) {
 		LOG_DEBUG("HTTP GET Status = %d, content_length = %d",
				esp_http_client_get_status_code(client),
				esp_http_client_get_content_length(client));
		LOG_INFO("\n%s", local_response_buffer);
		id = strtol(local_response_buffer, NULL, 10);

		//copy result into user buffer
		size_t resplen = strlen(local_response_buffer);
		if(resplen >= buflen) {
			LOG_ERROR("buffer not big enough to store HTTP response:\nbuffer: %ld, content_length: %d", buflen, esp_http_client_get_content_length(client));
			return ERR_C_MEMORY_ERR;
		}

		mempcpy(buffer, local_response_buffer, resplen);
		LOG_DEBUG("HTTP response stored into buffer");
    } else {
        LOG_ERROR("HTTP GET request failed: %s", esp_err_to_name(err));
    }
	esp_http_client_cleanup(client);
    return (int)id;
}