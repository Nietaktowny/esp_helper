#include "unity.h"
#include <stdio.h>
#include <pthread.h>

#include "logger.h"

struct logger_contex {
    FILE* logfiles[LOGGER_MAX_OUTPUT_FILES];
    pthread_mutex_t mutex;
    uint8_t level;
    uint8_t logfiles_num;
    uint8_t silent_mode;
};

#define LOG_TEST_NAME                                                                  \
    printf(LOG_VERBOSE_FORMAT, RED_BOLD_UNTERLINE, timenow(), "TEST_NAME", __FILE__, __LINE__, __func__, __LINE__); \
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

int runUnityTests(void) {
  UNITY_BEGIN();
 // RUN_TEST(test_if_clear_all_files_sets_all_files_to_null);
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