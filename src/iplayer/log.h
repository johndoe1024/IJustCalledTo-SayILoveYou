#pragma once

#ifdef IPLAYER_ENABLE_LOG

#include <stdio.h>

// TODO: LOG/TRACE are not thread safe
#define LOG(fmt, ...)                                                         \
  do {                                                                        \
    fprintf(stderr, fmt " [%s:%d:%s()]\n", ##__VA_ARGS__, __FILE__, __LINE__, \
            __func__);                                                        \
  } while (0)

#define TRACE() LOG("")

#else

#define LOG(...)
#define TRACE(...)

#endif  // IPLAYER_ENABLE_LOG
