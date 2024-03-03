#include <esp_http_server.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <unistd.h>

#include "err_controller.h"
#include "esp_check.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "esp_tls_crypto.h"
#include "logger.h"
#include "memory_utils.h"
#include "spiffs_controller.h"
#include "wifi_controller.h"
#include "wifi_manager.h"
#include "wifi_manager_internal.h"

// const httpd related values stored in ROM
const static char http_200_hdr[] = "200 OK";
// const static char http_302_hdr[] = "302 Found";
// const static char http_400_hdr[] = "400 Bad Request";
// const static char http_404_hdr[] = "404 Not Found";
const static char http_503_hdr[] = "503 Service Unavailable";
// const static char http_location_hdr[] = "Location";
const static char http_content_type_html[] = "text/html";
const static char http_content_type_js[] = "text/javascript";
const static char http_content_type_css[] = "text/css";
const static char http_content_type_json[] = "application/json";
const static char http_cache_control_hdr[] = "Cache-Control";
const static char http_cache_control_no_cache[] =
    "no-store, no-cache, must-revalidate, max-age=0";
const static char http_cache_control_cache[] = "public, max-age=31536000";
const static char http_pragma_hdr[] = "Pragma";
const static char http_pragma_no_cache[] = "no-cache";

/**
 * @brief embedded binary data.
 * @see file "platformio.ini"
 * @see
 * https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html#embedding-binary-data
 * @see
 * https://docs.platformio.org/en/latest/platforms/espressif32.html#embedding-binary-data
 */
extern const uint8_t style_css_start[] asm("_binary_styles_css_start");
extern const uint8_t style_css_end[] asm("_binary_styles_css_end");
extern const uint8_t code_js_start[] asm("_binary_code_js_start");
extern const uint8_t code_js_end[] asm("_binary_code_js_end");
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

struct wifi_s_handle_obj {
    httpd_handle_t esp_handle;
    httpd_config_t esp_config;
};

/* Our URI handler function to be called during GET /uri request */
esp_err_t root_handler(httpd_req_t* req) {
    LOG_VERBOSE("serving GET \"/\" request...");
    httpd_resp_set_status(req, http_200_hdr);
    httpd_resp_set_type(req, http_content_type_html);
    httpd_resp_send(req, (char*)index_html_start, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* Our URI handler function to be called during GET /styles.css request */
esp_err_t stylesheet_handler(httpd_req_t* req) {
    LOG_VERBOSE("serving GET \"/styles.css\" request...");
    httpd_resp_set_status(req, http_200_hdr);
    httpd_resp_set_type(req, http_content_type_css);
    httpd_resp_set_hdr(req, http_cache_control_hdr, http_cache_control_cache);
    httpd_resp_send(req, (char*)style_css_start, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* Our URI handler function to be called during GET /code.js request */
esp_err_t script_handler(httpd_req_t* req) {
    LOG_VERBOSE("serving GET \"/code.js\" request...");
    httpd_resp_set_status(req, http_200_hdr);
    httpd_resp_set_type(req, http_content_type_js);
    httpd_resp_send(req, (char*)code_js_start, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* Our URI handler function to be called during GET /scan request */
esp_err_t ap_json_handler(httpd_req_t* req) {
    err_c_t err = 0;
    char buffer[2096] = "Scanned AP: none";
    LOG_VERBOSE("serving GET \"/scan\" request...");

    err = wifi_manager_get_scanned_aps(&buffer[0], sizeof(buffer));

    httpd_resp_set_status(req, http_200_hdr);
    httpd_resp_set_type(req, http_content_type_json);
    httpd_resp_set_hdr(req, http_cache_control_hdr,
		       http_cache_control_no_cache);
    httpd_resp_set_hdr(req, http_pragma_hdr, http_pragma_no_cache);
    httpd_resp_send(req, &buffer[0], HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* Our URI handler function to be called during GET /state request */
esp_err_t state_json_handler(httpd_req_t* req) {
    err_c_t err = 0;
    char buffer[1096];
    LOG_VERBOSE("serving GET \"/state\" request...");

    err = wifi_c_get_status_as_json(&buffer[0], sizeof(buffer));

    httpd_resp_set_status(req, http_200_hdr);
    httpd_resp_set_type(req, http_content_type_json);
    httpd_resp_set_hdr(req, http_cache_control_hdr,
		       http_cache_control_no_cache);
    httpd_resp_set_hdr(req, http_pragma_hdr, http_pragma_no_cache);
    httpd_resp_send(req, &buffer[0], HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* Our URI handler function to be called during POST /connect request */
esp_err_t connect_handler(httpd_req_t* req) {
    err_c_t err = 0;
    char buffer[256];
    char ssid[64];
    char password[64];
    size_t ssid_len = 0;
    size_t password_len = 0;

    memutil_zero_memory(&buffer, sizeof(buffer));
    LOG_VERBOSE("serving POST \"/connect\" request...");

    err = httpd_req_recv(req, buffer, sizeof(buffer));
    if (err <= 0) {
	if (err == HTTPD_SOCK_ERR_TIMEOUT) {
	    LOG_WARN(
		"timeout on \"connect\" request, sending HTTP 408 (Request "
		"Timeout)");
	    httpd_resp_send_408(req);
	}

	return ESP_FAIL;
    }

    // get ssid header
    ssid_len = httpd_req_get_hdr_value_len(req, "X-Custom-ssid");
    if (!ssid_len) {
	LOG_ERROR("Length of X-Custom-ssid header cannot be 0");
	return ESP_FAIL;
    } else if (ssid_len > sizeof(ssid)) {
	LOG_ERROR("Length of X-Custom-ssid header bigger than buffer: %d",
		  ssid_len);
	return ESP_FAIL;
    }
    httpd_req_get_hdr_value_str(req, "X-Custom-ssid", ssid, ssid_len + 1);

    // get password header
    password_len = httpd_req_get_hdr_value_len(req, "X-Custom-password");
    if (!password_len) {
	LOG_ERROR("Length of X-Custom-password header cannot be 0");
	return ESP_FAIL;
    } else if (password_len > sizeof(password)) {
	LOG_ERROR("Length of X-Custom-password header bigger than buffer: %d",
		  password_len);
	return ESP_FAIL;
    }
    httpd_req_get_hdr_value_str(req, "X-Custom-password", password,
				password_len + 1);

    LOG_INFO("Received APs credentials!\n\tSSID: %s\n\tPassword: %s", ssid,
	     password);

    httpd_resp_set_status(req, http_200_hdr);
    httpd_resp_send(req,
		    "Got access point credentials, device will now restart!",
		    HTTPD_RESP_USE_STRLEN);
    LOG_WARN("Got access point credentials, device will now restart!");

    wifi_manager_store_ap(ssid, ssid_len, password, password_len);
    vTaskDelay(pdMS_TO_TICKS(100));

    esp_restart();
    return ESP_OK;
}

/* Our URI handler function to be called during GET /saved_ap request */
esp_err_t saved_ap_json_handler(httpd_req_t* req) {
    err_c_t err = 0;
    char buffer[128];

    LOG_VERBOSE("serving GET \"/saved_ap\" request...");

    err = wifi_manager_get_stored_ap_as_json(&buffer[0], sizeof(buffer));
    if (err != ERR_C_OK && err != 4354) {
	LOG_ERROR(
	    "cannot server GET \"/saved_ap\" request, error when generating "
	    "stored AP JSON response body...");
	return ESP_FAIL;
    }

    httpd_resp_set_status(req, http_200_hdr);
    httpd_resp_set_type(req, http_content_type_json);
    httpd_resp_set_hdr(req, http_cache_control_hdr,
		       http_cache_control_no_cache);
    httpd_resp_set_hdr(req, http_pragma_hdr, http_pragma_no_cache);
    httpd_resp_send(req, &buffer[0], HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* Our URI handler function to be called during GET /restart request */
esp_err_t restart_handler(httpd_req_t* req) {
    LOG_VERBOSE("serving GET \"/restart\" request...");
    httpd_resp_set_status(req, http_200_hdr);
    httpd_resp_send(req, "Device will now restart!", HTTPD_RESP_USE_STRLEN);
    vTaskDelay(pdMS_TO_TICKS(1000));
    LOG_WARN("Device will now restart!");

    esp_restart();

    return ESP_OK;
}

/* Our URI handler function to be called during GET /clear request */
esp_err_t clear_handler(httpd_req_t* req) {
    err_c_t err = 0;
    LOG_VERBOSE("serving GET \"/clear\" request...");

    err = wifi_manager_erase_ap();
    if (err != ERR_C_OK) {
	LOG_ERROR("cannot erase stored AP in NVS, error %d : %s", err,
		  error_to_name(err));
	httpd_resp_set_status(req, http_503_hdr);
	httpd_resp_send(req, "Cannot clear stored APs!", HTTPD_RESP_USE_STRLEN);
    }

    httpd_resp_set_status(req, http_200_hdr);
    httpd_resp_send(req, "All saved APs were cleared!", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* URI handler structure for GET / */
httpd_uri_t uri_get_root = {
    .uri = "/", .method = HTTP_GET, .handler = root_handler, .user_ctx = NULL};

/* URI handler structure for GET /styles.css */
httpd_uri_t uri_get_styles = {.uri = "/styles.css",
			      .method = HTTP_GET,
			      .handler = stylesheet_handler,
			      .user_ctx = NULL};

/* URI handler structure for GET /code.js */
httpd_uri_t uri_get_script = {.uri = "/code.js",
			      .method = HTTP_GET,
			      .handler = script_handler,
			      .user_ctx = NULL};

/*URI handler structure for GET /scan */
httpd_uri_t uri_get_ap_json = {.uri = "/scan",
			       .method = HTTP_GET,
			       .handler = ap_json_handler,
			       .user_ctx = NULL};

/*URI handler structure for GET /state */
httpd_uri_t uri_get_state_json = {.uri = "/state",
				  .method = HTTP_GET,
				  .handler = state_json_handler,
				  .user_ctx = NULL};

/*URI handler structure for POST /connect */
httpd_uri_t uri_post_connect = {.uri = "/connect",
				.method = HTTP_POST,
				.handler = connect_handler,
				.user_ctx = NULL};

/*URI handler structure for GET /saved_ap */
httpd_uri_t uri_get_saved_ap_json = {.uri = "/saved_ap",
				     .method = HTTP_GET,
				     .handler = saved_ap_json_handler,
				     .user_ctx = NULL};

/*URI handler structure for GET /restart */
httpd_uri_t uri_get_restart = {.uri = "/restart",
			       .method = HTTP_GET,
			       .handler = restart_handler,
			       .user_ctx = NULL};

/*URI handler structure for GET /clear */
httpd_uri_t uri_get_clear = {.uri = "/clear",
			     .method = HTTP_GET,
			     .handler = clear_handler,
			     .user_ctx = NULL};

int wifi_manager_server_init(wifi_s_handle_t* out_handle) {
    err_c_t err = 0;
    httpd_config_t default_config = HTTPD_DEFAULT_CONFIG();
    default_config.stack_size = 9046;
    default_config.send_wait_timeout = 20;
    default_config.max_resp_headers = 1024;
    default_config.max_uri_handlers = 9;
    httpd_handle_t temp_esp_handle = NULL;
    ERR_C_CHECK_NULL_PTR(
	out_handle,
	LOG_ERROR(
	    "address to store wifi manager http server handle cannot be NULL"));

    // alloc memory
    NEW((*out_handle), struct wifi_s_handle_obj);

    err = httpd_start(&temp_esp_handle, &default_config);
    if (err != ERR_C_OK) {
	LOG_ERROR("error %d during starting http server: %s", err,
		  error_to_name(err));
	return err;
    }

    httpd_register_uri_handler(temp_esp_handle, &uri_get_script);
    httpd_register_uri_handler(temp_esp_handle, &uri_get_styles);
    httpd_register_uri_handler(temp_esp_handle, &uri_get_root);
    httpd_register_uri_handler(temp_esp_handle, &uri_get_ap_json);
    httpd_register_uri_handler(temp_esp_handle, &uri_get_state_json);
    httpd_register_uri_handler(temp_esp_handle, &uri_post_connect);
    httpd_register_uri_handler(temp_esp_handle, &uri_get_saved_ap_json);
    httpd_register_uri_handler(temp_esp_handle, &uri_get_clear);
    httpd_register_uri_handler(temp_esp_handle, &uri_get_restart);

    // alles ging gut, copy
    memcpy(&default_config, &((*out_handle)->esp_config),
	   sizeof(httpd_config_t));
    (*out_handle)->esp_handle = NULL;

    LOG_DEBUG("http server started");
    return err;
}
