#pragma once
#include "ansi_colors.h"
#include <stdio.h>
#include "stdbool.h"

extern int iDebugLevel;

#define SET_LOG_LEVEL(level) int iDebugLevel = level


#define LOG_LEVEL_FATAL (1)
#define LOG_LEVEL_ERROR (2)
#define LOG_LEVEL_WARN (3)
#define LOG_LEVEL_INFO (4)
#define LOG_LEVEL_DEBUG (5)

#define LOG_FATAL(...)                                                          \
  do {                                                                          \
    if (iDebugLevel >= LOG_LEVEL_FATAL) {                                       \
      fprintf(stderr, "%s%s:%d:",RED_BOLD_UNTERLINE, __FILE__, __LINE__);       \
      fprintf(stderr, __VA_ARGS__);                                             \
      fprintf(stderr, "%s\n", RESET);                                           \
      fflush(stderr);                                                           \
    }                                                                           \
  } while (0)                                               

#define LOG_ERROR(...)                                                         \
  do {                                                                         \
    if (iDebugLevel >= LOG_LEVEL_ERROR) {                                      \
      fprintf(stderr, "%s%s:%d:", RED, __FILE__, __LINE__);                    \
      fprintf(stderr, __VA_ARGS__);                                            \
      fprintf(stderr, "%s\n", RESET);                                          \
      fflush(stderr);                                                          \
    }                                                                          \
  } while (0)                               

#define LOG_WARN(...)                                                          \
  do {                                                                         \
    if (iDebugLevel >= LOG_LEVEL_WARN) {                                       \
      fprintf(stderr, "%s%s:%d:", YELLOW, __FILE__, __LINE__);                 \
      fprintf(stderr, __VA_ARGS__);                                            \
      fprintf(stderr, "%s\n", RESET);                                          \
      fflush(stderr);                                                          \
    }                                                                          \
  } while (0)                               


#define LOG_INFO(...)                                                          \
  do {                                                                         \
    if (iDebugLevel >= LOG_LEVEL_INFO) {                                       \
      fprintf(stdout, "%s", GREEN);                                            \
      fprintf(stdout, __VA_ARGS__);                                            \
      fprintf(stdout, "%s\n", RESET);                                          \
      fflush(stdout);                                                          \
    }                                                                          \
  } while (0)                               


#define LOG_DEBUG(...)                                                         \
  do {                                                                         \
    if (iDebugLevel >= LOG_LEVEL_DEBUG) {                                      \
      fprintf(stdout, __VA_ARGS__);                                            \
      fprintf(stdout, "%s\n", RESET);                                          \
      fflush(stdout);                                                          \
    }                                                                          \
  } while (0)

  