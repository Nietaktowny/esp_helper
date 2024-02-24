#include "unity.h"
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "logger.h"

struct logger_contex {
    FILE* logfiles[LOGGER_MAX_OUTPUT_FILES];
    pthread_mutex_t mutex;
    uint8_t level;
    uint8_t logfiles_num;
    uint8_t silent_mode;
};

#define LOG_TEST_NAME                                                                  \
    printf(LOG_VERBOSE_FORMAT, PURPLE_BOLD, timenow(), "TEST_NAME", __FILE__, __LINE__, __func__, __LINE__); \
    printf("%s\n", RESET);                                                                         \
    fflush(stdout)

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

void test_if_log_verbose_format_is_defined(void) {
    //before
    LOG_TEST_NAME;

    //given
    const char* verbose_format_expected = "%s%s | %-5s | %-15s:%d | %s:%d | ";

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(verbose_format_expected, LOG_VERBOSE_FORMAT, "LOG_VERBOSE_FORMAT different from expected.");
}

void test_if_log_strict_format_is_defined(void) {
    //before
    LOG_TEST_NAME;

    //given
    const char* verbose_format_expected = "%s%s | %-5s | %s:%d | ";

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(verbose_format_expected, LOG_STRICT_FORMAT, "LOG_STRICT_FORMAT different from expected.");
}

void test_if_logger_create_semphr_returns_zero(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = -1;

    //when
    err = logger_create_semphr();

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err , "logger_create_semphr should return 0 on success");
}

void test_if_logger_create_semphr_creates_mutex(void) {
    //before
    LOG_TEST_NAME;

    //given
    pthread_mutex_t* mutex = NULL;

    //when
    logger_create_semphr();
    mutex = (pthread_mutex_t*)logger_get_log_mutex();

    //then
    TEST_ASSERT_NOT_NULL_MESSAGE(mutex, "logger mutex cannot be NULL after logger_create_semphr");
}

void test_if_logger_delete_semphr_deletes_mutex(void) {
    //before
    LOG_TEST_NAME;

    //given
    logger_create_semphr();
    pthread_mutex_t* mutex = (pthread_mutex_t*)logger_get_log_mutex();
    TEST_ASSERT_NOT_NULL_MESSAGE(mutex, "logger mutex cannot be NULL");

    //when
    logger_delete_semphr();
    mutex = (pthread_mutex_t*)logger_get_log_mutex();

    //then
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, pthread_mutex_lock(mutex), "mutex should be destroyed after logger delete_sempthr");
}

void test_if_logger_init_returns_zero(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = 0;

    //when
    err = logger_init();

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "logger_init should return 0");    
}

void test_if_after_logger_init_log_level_is_debug(void) {
    //before
    LOG_TEST_NAME;

    //given
    struct logger_contex* logger = NULL;
    uint8_t debug_level = -1;

    //when
    logger_init();
    logger = logger_get_logger_contex();
    debug_level = logger->level;

    //then
    TEST_ASSERT_EQUAL_MESSAGE(LOG_LEVEL_DEBUG, debug_level, "log level should be set to LOG_LEVEL_DEBUG after logger init");    
}

void test_if_after_logger_init_silent_mode_is_disabled(void) {
    //before
    LOG_TEST_NAME;

    //given
    struct logger_contex* logger = NULL;
    uint8_t silent_mode = -1;

    //when
    logger_init();
    logger = logger_get_logger_contex();
    silent_mode = logger->silent_mode;

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, silent_mode, "silent mode should be disabled after logger init");    
}

void test_if_after_logger_add_file_appends_file(void) {
    //before
    LOG_TEST_NAME;

    //given
    logger_create_semphr();
    struct logger_contex* logger = NULL;
    FILE* file = fopen("new_file.txt", "w");

    //when
    logger_init();
    logger_add_log_file(file);
    logger = logger_get_logger_contex();

    //then
    TEST_ASSERT_EQUAL_MESSAGE(file, logger->logfiles[logger->logfiles_num -1], "log file should be added after logger_add_log_file");

    //after
    logger_clear_all_log_files();
    remove("new_file.txt");
}

void test_if_after_logger_add_file_files_num_changes(void) {
    //before
    LOG_TEST_NAME;

    //given
    struct logger_contex* logger = logger_get_logger_contex();
    int logfiles_num_before = logger->logfiles_num;
    FILE* file = fopen("new_file2.txt", "w");

    //when
    logger_init();
    logger_add_log_file(file);
    logger = logger_get_logger_contex();

    //then
    TEST_ASSERT_EQUAL_MESSAGE(++logfiles_num_before, logger->logfiles_num, "logfiles_num should be 1 after logger_add_log_file adds one file");

    //after
    logger_clear_all_log_files();
    remove("new_file2.txt");   
}

void test_if_after_logger_add_file_return_err_on_null_file(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = 0;
    FILE* file = NULL;

    //when
    logger_init();
    err = logger_add_log_file(file);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(LOGGER_ERR_NULL_FILE, err, "logger_add_log_file should return LOGGER_ERR_NULL_FILE on null file"); 
}

void test_if_clear_all_files_sets_filesnum_to_zero(void) {
    //before
    LOG_TEST_NAME;

    //given
    struct logger_contex* logger = logger_get_logger_contex();

    //when
    logger_init();
    logger->logfiles_num = 2;
    logger_clear_all_log_files();
    

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, logger->logfiles_num, "logfiles_num should be zero after logger_clear_all_log_files"); 
}

void test_if_clear_all_files_sets_all_files_to_null(void) {
    //before
    LOG_TEST_NAME;

    //given
    FILE* file = fopen("new_file.txt", "w");
    FILE* file2 = fopen("new_file2.txt", "w");
    FILE* file3 = fopen("new_file3.txt", "w");
    FILE* file4 = fopen("new_file4.txt", "w");
    FILE* file5 = fopen("new_file5.txt", "w");
    struct logger_contex* logger = logger_get_logger_contex();
    int err = -1;

    //when
    logger_init();
    err =logger_add_log_file(file);
    TEST_ASSERT_EQUAL(0, err);
    err = logger_add_log_file(file2);
    TEST_ASSERT_EQUAL(0, err);
    err = logger_add_log_file(file3);
    TEST_ASSERT_EQUAL(0, err);
    err = logger_add_log_file(file4);
    TEST_ASSERT_EQUAL(0, err);
    err = logger_add_log_file(file5);
    logger_clear_all_log_files();
    

    //then
    for (uint8_t i = 0; i < logger->logfiles_num; i++)
    {
        TEST_ASSERT_NULL_MESSAGE(logger->logfiles[i], "after logger_clear_all_log_files all log files should be NULL");
    }
    
    //after
    remove("new_file.txt");
    remove("new_file2.txt");
    remove("new_file3.txt");
    remove("new_file4.txt");
    remove("new_file5.txt");
}

void test_if_verbose_level_num_is_defined(void) {
    //before
    LOG_TEST_NAME;

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0x06, LOG_LEVEL_VERBOSE, "LOG_LEVEL_VERBOSE value different from expected.");
}

void test_if_debug_level_num_is_defined(void) {
    //before
    LOG_TEST_NAME;

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0x05, LOG_LEVEL_DEBUG, "LOG_LEVEL_DEBUG value different from expected.");
}

void test_if_info_level_num_is_defined(void) {
    //before
    LOG_TEST_NAME;

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0x04, LOG_LEVEL_INFO, "LOG_LEVEL_INFO value different from expected.");
}

void test_if_warn_level_num_is_defined(void) {
    //before
    LOG_TEST_NAME;

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0x03, LOG_LEVEL_WARN, "LOG_LEVEL_WARN value different from expected.");
}

void test_if_error_level_num_is_defined(void) {
    //before
    LOG_TEST_NAME;

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0x02, LOG_LEVEL_ERROR, "LOG_LEVEL_ERROR value different from expected.");
}

void test_if_fatal_level_num_is_defined(void) {
    //before
    LOG_TEST_NAME;

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0x01, LOG_LEVEL_FATAL, "LOG_LEVEL_FATAL value different from expected.");
}

void test_if_zero_level_num_is_defined(void) {
    //before
    LOG_TEST_NAME;

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0x00, LOG_LEVEL_ZERO, "LOG_LEVEL_ZERO value different from expected.");
}

void test_if_set_log_level_macro_is_defined_to_verbose(void) {
    //before
    LOG_TEST_NAME;

    //then
    TEST_ASSERT_EQUAL_MESSAGE(LOG_LEVEL_VERBOSE, SET_LOG_LEVEL, "Set log level should be set to LOG_LEVEL_VERBOSE");
}

void test_if_time_log_format_is_defined(void) {
    //before
    LOG_TEST_NAME;

    //given
    const char* expected = "%Y-%m-%d %H:%M:%S";

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, LOG_TIME_FORMAT, "LOG_TIME_FORMAT different from expected.");
}

void test_if_logger_write_returns_zero_on_lower_log_level(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = -1;

    //when
    logger_set_log_level(LOG_LEVEL_DEBUG);
    err = logger_write(LOG_LEVEL_VERBOSE, "test");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "logger_write should return 0 if log level lower than currently set");
}

void test_if_logger_write_returns_non_zero_on_higher_log_level(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = -1;

    //when
    logger_set_log_level(LOG_LEVEL_INFO);
    err = logger_write(LOG_LEVEL_ERROR, "test");

    //then
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, err, "logger_write should return non zero if log level is higher than currently set");
}

void test_if_logger_write_returns_expected_num_of_chars(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = -1;
    const char* log = "test_string";
    int expected = strlen(log);

    //when
    logger_set_log_level(LOG_LEVEL_ERROR);
    err = logger_write(LOG_LEVEL_FATAL, "%s", log);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(expected, err, "logger_write should return number of chars written.");
}

void test_if_logger_write_returns_non_zero_on_the_same_log_level_debug(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = -1;

    //when
    logger_set_log_level(LOG_LEVEL_DEBUG);
    err = logger_write(LOG_LEVEL_DEBUG, "test_log");

    //then
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, err, "logger_write should return number of chars written on the same log level as set");
}

void test_if_logger_write_returns_non_zero_on_the_same_log_level_verbose(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = -1;

    //when
    logger_set_log_level(LOG_LEVEL_VERBOSE);
    err = logger_write(LOG_LEVEL_VERBOSE, "test_log");

    //then
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, err, "logger_write should return number of chars written on the same log level as set");
}

void test_if_logger_write_returns_non_zero_on_the_same_log_level_info(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = -1;

    //when
    logger_set_log_level(LOG_LEVEL_INFO);
    err = logger_write(LOG_LEVEL_INFO, "test_log");

    //then
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, err, "logger_write should return number of chars written on the same log level as set");
}

void test_if_logger_write_returns_non_zero_on_the_same_log_level_warn(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = -1;

    //when
    logger_set_log_level(LOG_LEVEL_WARN);
    err = logger_write(LOG_LEVEL_WARN, "test_log");

    //then
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, err, "logger_write should return number of chars written on the same log level as set");
}

void test_if_logger_write_returns_non_zero_on_the_same_log_level_error(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = -1;

    //when
    logger_set_log_level(LOG_LEVEL_ERROR);
    err = logger_write(LOG_LEVEL_ERROR, "test_log");

    //then
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, err, "logger_write should return number of chars written on the same log level as set");
}

void test_if_logger_write_returns_non_zero_on_the_same_log_level_fatal(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = -1;

    //when
    logger_set_log_level(LOG_LEVEL_FATAL);
    err = logger_write(LOG_LEVEL_FATAL, "test_log");

    //then
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, err, "logger_write should return number of chars written on the same log level as set");
}

void test_if_logger_write_returns_zero_when_log_level_is_zero_and_log_is_verbose(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = -1;

    //when
    logger_set_log_level(LOG_LEVEL_ZERO);
    err = logger_write(LOG_LEVEL_VERBOSE, "test_log");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "logger_write should return zero when log level is set to zero");
}

void test_if_logger_write_returns_zero_when_log_level_is_zero_and_log_is_debug(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = -1;

    //when
    logger_set_log_level(LOG_LEVEL_ZERO);
    err = logger_write(LOG_LEVEL_DEBUG, "test_log");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "logger_write should return zero when log level is set to zero");
}

void test_if_logger_write_returns_zero_when_log_level_is_zero_and_log_is_info(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = -1;

    //when
    logger_set_log_level(LOG_LEVEL_ZERO);
    err = logger_write(LOG_LEVEL_INFO, "test_log");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "logger_write should return zero when log level is set to zero");
}

void test_if_logger_write_returns_zero_when_log_level_is_zero_and_log_is_warn(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = -1;

    //when
    logger_set_log_level(LOG_LEVEL_ZERO);
    err = logger_write(LOG_LEVEL_WARN, "test_log");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "logger_write should return zero when log level is set to zero");
}

void test_if_logger_write_returns_zero_when_log_level_is_zero_and_log_is_error(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = -1;

    //when
    logger_set_log_level(LOG_LEVEL_ZERO);
    err = logger_write(LOG_LEVEL_ERROR, "test_log");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "logger_write should return zero when log level is set to zero");
}

void test_if_logger_write_returns_zero_when_log_level_is_zero_and_log_is_fatal(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = -1;

    //when
    logger_set_log_level(LOG_LEVEL_ZERO);
    err = logger_write(LOG_LEVEL_FATAL, "test_log");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "logger_write should return zero when log level is set to zero");
}

void test_if_logger_write_returns_zero_when_no_file_is_set_and_silent_mode_is_enabled(void) {
    //before
    LOG_TEST_NAME;

    //given
    int err = -1;

    //when
    logger_set_log_level(LOG_LEVEL_DEBUG);
    logger_enable_silent_mode();
    err = logger_write(LOG_LEVEL_FATAL, "test_log");

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "logger_write should return zero silent mode is enabled and no file is set.");

    //after
    logger_disable_silent_mode();
}

void test_if_logger_enable_silent_mode_returns_zero_if_mode_was_not_enabled(void) {
    //before
    LOG_TEST_NAME;

    //given
    int mode = -1;

    //when
    mode = logger_enable_silent_mode();

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, mode, "logger_enable_silent_mode should return zero if mode was before not enabled");

    //after
    logger_disable_silent_mode();
}

void test_if_logger_disable_silent_mode_returns_zero_if_mode_was_not_enabled(void) {
    //before
    LOG_TEST_NAME;

    //given
    int mode = -1;

    //when
    mode = logger_disable_silent_mode();

    //then
    TEST_ASSERT_EQUAL_MESSAGE(0, mode, "logger_disable_silent_mode should return zero if mode was before not enabled");
}

void test_if_logger_enable_silent_mode_returns_non_zero_if_mode_was_enabled(void) {
    //before
    LOG_TEST_NAME;

    //given
    int mode = -1;

    //when
    logger_enable_silent_mode();
    mode = logger_enable_silent_mode();

    //then
    TEST_ASSERT_EQUAL_MESSAGE(1, mode, "logger_enable_silent_mode should return 1 if mode was before enabled");

    //after
    logger_disable_silent_mode();
}

void test_if_logger_disable_silent_mode_returns_non_zero_if_mode_was_enabled(void) {
    //before
    LOG_TEST_NAME;

    //given
    int mode = -1;

    //when
    logger_enable_silent_mode();
    mode = logger_disable_silent_mode();

    //then
    TEST_ASSERT_EQUAL_MESSAGE(1, mode, "logger_disable_silent_mode should return 1 if mode was before enabled");
}

void test_if_logger_set_log_level_returns_previous_log_level(void) {
    //before
    LOG_TEST_NAME;

    //given
    int level = -1;
    struct logger_contex* logger = logger_get_logger_contex();
    int expected = logger->level;

    //when
    level = logger_set_log_level(LOG_LEVEL_FATAL);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(expected, level, "logger_set_log_level returned different log level than previous");
}

void test_if_logger_set_log_level_changes_log_level_to_verbose(void) {
    //before
    LOG_TEST_NAME;

    //given
    struct logger_contex* logger = logger_get_logger_contex();

    //when
    logger_set_log_level(LOG_LEVEL_VERBOSE);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(LOG_LEVEL_VERBOSE, logger->level, "logger_set_log_level set log level to LOG_LEVEL_VERBOSE");
}

void test_if_logger_set_log_level_changes_log_level_to_debug(void) {
    //before
    LOG_TEST_NAME;

    //given
    struct logger_contex* logger = logger_get_logger_contex();

    //when
    logger_set_log_level(LOG_LEVEL_DEBUG);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(LOG_LEVEL_DEBUG, logger->level, "logger_set_log_level set log level to LOG_LEVEL_DEBUG");
}

void test_if_logger_set_log_level_changes_log_level_to_info(void) {
    //before
    LOG_TEST_NAME;

    //given
    struct logger_contex* logger = logger_get_logger_contex();

    //when
    logger_set_log_level(LOG_LEVEL_INFO);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(LOG_LEVEL_INFO, logger->level, "logger_set_log_level set log level to LOG_LEVEL_INFO");
}

void test_if_logger_set_log_level_changes_log_level_to_warn(void) {
    //before
    LOG_TEST_NAME;

    //given
    struct logger_contex* logger = logger_get_logger_contex();

    //when
    logger_set_log_level(LOG_LEVEL_WARN);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(LOG_LEVEL_WARN, logger->level, "logger_set_log_level set log level to LOG_LEVEL_WARN");
}

void test_if_logger_set_log_level_changes_log_level_to_error(void) {
    //before
    LOG_TEST_NAME;

    //given
    struct logger_contex* logger = logger_get_logger_contex();

    //when
    logger_set_log_level(LOG_LEVEL_ERROR);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(LOG_LEVEL_ERROR, logger->level, "logger_set_log_level set log level to LOG_LEVEL_ERROR");
}

void test_if_logger_set_log_level_changes_log_level_to_fatal(void) {
    //before
    LOG_TEST_NAME;

    //given
    struct logger_contex* logger = logger_get_logger_contex();

    //when
    logger_set_log_level(LOG_LEVEL_FATAL);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(LOG_LEVEL_FATAL, logger->level, "logger_set_log_level set log level to LOG_LEVEL_FATAL");
}

void test_if_logger_set_log_level_changes_log_level_to_zero(void) {
    //before
    LOG_TEST_NAME;

    //given
    struct logger_contex* logger = logger_get_logger_contex();

    //when
    logger_set_log_level(LOG_LEVEL_ZERO);

    //then
    TEST_ASSERT_EQUAL_MESSAGE(LOG_LEVEL_ZERO, logger->level, "logger_set_log_level set log level to LOG_LEVEL_ZERO");
}

void test_if_default_verbose_color_is_defined(void) {
    //before
    LOG_TEST_NAME;

    //given
    const char* default_color = "\e[0;37m";

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(default_color, VERBOSE_COLOR, "verbose default color should be set to WHITE");
}

void test_if_default_debug_color_is_defined(void) {
    //before
    LOG_TEST_NAME;

    //given
    const char* default_color = "\e[0;34m";

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(default_color, DEBUG_COLOR, "debug default color should be set to BLUE");
}

void test_if_default_info_color_is_defined(void) {
    //before
    LOG_TEST_NAME;

    //given
    const char* default_color = "\e[0;32m";

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(default_color, INFO_COLOR, "info default color should be set to GREEN");
}

void test_if_default_warn_color_is_defined(void) {
    //before
    LOG_TEST_NAME;

    //given
    const char* default_color = "\e[0;33m";

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(default_color, WARN_COLOR, "warn default color should be set to YELLOW");
}

void test_if_default_error_color_is_defined(void) {
    //before
    LOG_TEST_NAME;

    //given
    const char* default_color = "\e[0;31m";

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(default_color, ERROR_COLOR, "error default color should be set to RED");
}

void test_if_default_fatal_color_is_defined(void) {
    //before
    LOG_TEST_NAME;

    //given
    const char* default_color = "\e[31;1;4m";

    //then
    TEST_ASSERT_EQUAL_STRING_MESSAGE(default_color, FATAL_COLOR, "fatal default color should be set to RED_BOLD_UNDERLINE");
}

int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_if_default_fatal_color_is_defined);
  RUN_TEST(test_if_default_error_color_is_defined);
  RUN_TEST(test_if_default_warn_color_is_defined);
  RUN_TEST(test_if_default_info_color_is_defined);
  RUN_TEST(test_if_default_debug_color_is_defined);
  RUN_TEST(test_if_default_verbose_color_is_defined);
  RUN_TEST(test_if_logger_set_log_level_changes_log_level_to_zero);
  RUN_TEST(test_if_logger_set_log_level_changes_log_level_to_fatal);
  RUN_TEST(test_if_logger_set_log_level_changes_log_level_to_error);
  RUN_TEST(test_if_logger_set_log_level_changes_log_level_to_warn);
  RUN_TEST(test_if_logger_set_log_level_changes_log_level_to_info);
  RUN_TEST(test_if_logger_set_log_level_changes_log_level_to_debug);
  RUN_TEST(test_if_logger_set_log_level_changes_log_level_to_verbose);
  RUN_TEST(test_if_logger_set_log_level_returns_previous_log_level);
  RUN_TEST(test_if_logger_disable_silent_mode_returns_non_zero_if_mode_was_enabled);
  RUN_TEST(test_if_logger_enable_silent_mode_returns_non_zero_if_mode_was_enabled);
  RUN_TEST(test_if_logger_disable_silent_mode_returns_zero_if_mode_was_not_enabled);
  RUN_TEST(test_if_logger_enable_silent_mode_returns_zero_if_mode_was_not_enabled);
  RUN_TEST(test_if_logger_write_returns_zero_when_no_file_is_set_and_silent_mode_is_enabled);
  RUN_TEST(test_if_logger_write_returns_zero_when_log_level_is_zero_and_log_is_fatal);
  RUN_TEST(test_if_logger_write_returns_zero_when_log_level_is_zero_and_log_is_error);
  RUN_TEST(test_if_logger_write_returns_zero_when_log_level_is_zero_and_log_is_warn);
  RUN_TEST(test_if_logger_write_returns_zero_when_log_level_is_zero_and_log_is_info);
  RUN_TEST(test_if_logger_write_returns_zero_when_log_level_is_zero_and_log_is_debug);
  RUN_TEST(test_if_logger_write_returns_zero_when_log_level_is_zero_and_log_is_verbose);
  RUN_TEST(test_if_logger_write_returns_non_zero_on_the_same_log_level_fatal);
  RUN_TEST(test_if_logger_write_returns_non_zero_on_the_same_log_level_error);
  RUN_TEST(test_if_logger_write_returns_non_zero_on_the_same_log_level_warn);
  RUN_TEST(test_if_logger_write_returns_non_zero_on_the_same_log_level_info);
  RUN_TEST(test_if_logger_write_returns_non_zero_on_the_same_log_level_verbose);
  RUN_TEST(test_if_logger_write_returns_non_zero_on_the_same_log_level_debug);
  RUN_TEST(test_if_logger_write_returns_expected_num_of_chars);
  RUN_TEST(test_if_logger_write_returns_non_zero_on_higher_log_level);
  RUN_TEST(test_if_logger_write_returns_zero_on_lower_log_level);
  RUN_TEST(test_if_time_log_format_is_defined);
  RUN_TEST(test_if_set_log_level_macro_is_defined_to_verbose);
  RUN_TEST(test_if_zero_level_num_is_defined);
  RUN_TEST(test_if_fatal_level_num_is_defined);
  RUN_TEST(test_if_error_level_num_is_defined);
  RUN_TEST(test_if_warn_level_num_is_defined);
  RUN_TEST(test_if_info_level_num_is_defined);
  RUN_TEST(test_if_debug_level_num_is_defined);
  RUN_TEST(test_if_verbose_level_num_is_defined);
  RUN_TEST(test_if_clear_all_files_sets_filesnum_to_zero);
  RUN_TEST(test_if_after_logger_add_file_return_err_on_null_file);
  RUN_TEST(test_if_after_logger_add_file_files_num_changes);
  RUN_TEST(test_if_after_logger_add_file_appends_file);
  RUN_TEST(test_if_after_logger_init_silent_mode_is_disabled);
  RUN_TEST(test_if_after_logger_init_log_level_is_debug);
  RUN_TEST(test_if_logger_init_returns_zero);
  RUN_TEST(test_if_log_verbose_format_is_defined);
  RUN_TEST(test_if_log_strict_format_is_defined);
  RUN_TEST(test_if_logger_create_semphr_returns_zero);
  RUN_TEST(test_if_logger_create_semphr_creates_mutex);
  RUN_TEST(test_if_logger_delete_semphr_deletes_mutex);
  return UNITY_END();
}


/**
  * For native platform.
  */
int main(void) {
  runUnityTests();
}