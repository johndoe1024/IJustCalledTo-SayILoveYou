#pragma once

#ifdef IPLAYER_ENABLE_LOG

#include <stdio.h>

// TODO: log are not thread safe
#define LOG(fmt, ...)                                                      \
  do {                                                                     \
    fprintf(stderr, "%s:%d:%s(): " fmt "\n", __FILE__, __LINE__, __func__, \
            ##__VA_ARGS__);                                                \
  } while (0)

#define TRACE() LOG("")

#else

#define LOG(...)
#define TRACE(...)

#endif  // IPLAYER_ENABLE_LOG
