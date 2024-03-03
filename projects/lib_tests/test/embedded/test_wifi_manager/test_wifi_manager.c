#include <stdio.h>

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"

// Change fatal color to purple, to use it to log test names.
#include "ansi_colors.h"
#define FATAL_COLOR PURPLE_BOLD

#include <string.h>

#include "err_controller.h"
#include "logger.h"
#include "nvs_controller.h"
#include "wifi_controller.h"
#include "wifi_manager.h"
#include "wifi_manager_internal.h"

#define MY_SSID "TP-LINK_AD8313"
#define MY_PSK "20232887"
#define SOL_SSID "OstNet-952235"
#define SOL_PSK "Solonka106"
#define MI_SSID "Mi"
#define MI_PSK "Solonka106"

void setUp(void) {
	// set stuff up here
	logger_init();
	logger_set_log_level(LOG_LEVEL_VERBOSE);
}

void tearDown(void) {
	// clean stuff up here
}

void test_if_wifi_manager_store_ap_returns_err_on_null_ssid(void) {
	// before
	LOG_FATAL("RUNNING: %s", __func__);

	// given
	int err = ERR_C_OK;
	const char* pass = "password";

	// when
	err = wifi_manager_store_ap(NULL, 0, pass, strlen(pass));

	// then
	TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "wifi_manager_store_ap should return err on NULL ssid");
}

void test_if_wifi_manager_store_ap_returns_err_on_null_pass(void) {
	// before
	LOG_FATAL("RUNNING: %s", __func__);

	// given
	int err = ERR_C_OK;
	const char* ssid = "ssid";

	// when
	err = wifi_manager_store_ap(NULL, 0, ssid, strlen(ssid));

	// then
	TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "wifi_manager_store_ap should return err on NULL password");
}

void test_if_wifi_manager_store_ap_returns_err_on_not_init_nvs_partition(void) {
	// before
	LOG_FATAL("RUNNING: %s", __func__);

	// given
	int err = ERR_C_OK;
	const char* ssid = "ssid";
	const char* pass = "pass";

	// when
	nvs_c_deinit_default_partition();
	err = wifi_manager_store_ap(ssid, strlen(ssid), pass, strlen(pass));

	// then
	TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_NOT_INIT, err,
							  "wifi_manager_store_ap should return err on not "
							  "init default nvs partition");

	// after
	nvs_c_init_default_partition();
}

void test_if_wifi_manager_store_ap_returns_zero(void) {
	// before
	LOG_FATAL("RUNNING: %s", __func__);

	// given
	int err = ERR_C_OK;
	const char* ssid = "ssid";
	const char* pass = "pass";

	// when
	err = wifi_manager_store_ap(ssid, strlen(ssid), pass, strlen(pass));

	// then
	TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "wifi_manager_store_ap should return ERR_C_OK");
}

void test_if_wifi_manager_get_stored_ap_returns_err_on_null_ssid(void) {
	// before
	LOG_FATAL("RUNNING: %s", __func__);

	// given
	int err = ERR_C_OK;
	char pass[15] = {0};

	// when
	err = wifi_manager_get_stored_ap(NULL, 0, pass, sizeof(pass));

	// then
	TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "wifi_manager_get_stored_ap should return err on NULL ssid");
}

void test_if_wifi_manager_get_stored_ap_returns_err_on_null_pass(void) {
	// before
	LOG_FATAL("RUNNING: %s", __func__);

	// given
	int err = ERR_C_OK;
	char* ssid = {0};

	// when
	err = wifi_manager_get_stored_ap(NULL, 0, ssid, sizeof(ssid));

	// then
	TEST_ASSERT_EQUAL_MESSAGE(ERR_C_NULL_POINTER, err, "wifi_manager_get_stored_ap should return err on NULL password");
}

void test_if_wifi_manager_get_stored_ap_returns_err_on_not_init_nvs_partition(void) {
	// before
	LOG_FATAL("RUNNING: %s", __func__);

	// given
	int err = ERR_C_OK;
	char ssid[15] = {0};
	char pass[15] = {0};

	// when
	nvs_c_deinit_default_partition();
	err = wifi_manager_get_stored_ap(ssid, sizeof(ssid), pass, sizeof(pass));

	// then
	TEST_ASSERT_EQUAL_MESSAGE(NVS_C_ERR_NOT_INIT, err,
							  "wifi_manager_get_stored_ap should return err on "
							  "not init default nvs partition");

	// after
	nvs_c_init_default_partition();
}

void test_if_wifi_manager_get_stored_ap_returns_zero(void) {
	// before
	LOG_FATAL("RUNNING: %s", __func__);

	// given
	int err = ERR_C_OK;
	char ssid[15] = {0};
	char pass[15] = {0};

	// when
	err = wifi_manager_get_stored_ap(ssid, sizeof(ssid), pass, sizeof(pass));

	// then
	TEST_ASSERT_EQUAL_MESSAGE(ERR_C_OK, err, "wifi_manager_get_stored_ap should return ERR_C_OK");
}

void test_if_wifi_manager_gets_stored_ap_ssid(void) {
	// before
	LOG_FATAL("RUNNING: %s", __func__);

	// given
	const char* exp_ssid = "ssid";
	const char* exp_pass = "pass";
	char ssid[15] = {0};
	char pass[15] = {0};

	// when
	wifi_manager_store_ap(exp_ssid, strlen(exp_ssid), exp_pass, strlen(exp_pass));
	wifi_manager_get_stored_ap(ssid, sizeof(ssid), pass, sizeof(pass));

	// then
	TEST_ASSERT_EQUAL_STRING_MESSAGE(exp_ssid, ssid, "stored ssid different from expected");
}

void test_if_wifi_manager_gets_stored_ap_pass(void) {
	// before
	LOG_FATAL("RUNNING: %s", __func__);

	// given
	const char* exp_ssid = "ssid";
	const char* exp_pass = "pass";
	char ssid[15] = {0};
	char pass[15] = {0};

	// when
	wifi_manager_store_ap(exp_ssid, strlen(exp_ssid), exp_pass, strlen(exp_pass));
	wifi_manager_get_stored_ap(ssid, sizeof(ssid), pass, sizeof(pass));

	// then
	TEST_ASSERT_EQUAL_STRING_MESSAGE(exp_pass, pass, "stored pass different from expected");
}

int runUnityTests(void) {
	UNITY_BEGIN();
	RUN_TEST(test_if_wifi_manager_gets_stored_ap_pass);
	RUN_TEST(test_if_wifi_manager_gets_stored_ap_ssid);
	RUN_TEST(test_if_wifi_manager_get_stored_ap_returns_zero);
	RUN_TEST(test_if_wifi_manager_get_stored_ap_returns_err_on_not_init_nvs_partition);
	RUN_TEST(test_if_wifi_manager_get_stored_ap_returns_err_on_null_pass);
	RUN_TEST(test_if_wifi_manager_get_stored_ap_returns_err_on_null_ssid);
	RUN_TEST(test_if_wifi_manager_store_ap_returns_zero);
	RUN_TEST(test_if_wifi_manager_store_ap_returns_err_on_not_init_nvs_partition);
	RUN_TEST(test_if_wifi_manager_store_ap_returns_err_on_null_pass);
	RUN_TEST(test_if_wifi_manager_store_ap_returns_err_on_null_ssid);
	return UNITY_END();
}

/**
 * For ESP-IDF framework
 */
void app_main(void) {
	nvs_c_init_default_partition();
	wifi_c_init_wifi(WIFI_C_MODE_STA);
	wifi_c_start_sta(MY_SSID, MY_PSK);
	runUnityTests();
	vTaskDelay(200);
	fflush(stdout);
	esp_restart();
}
