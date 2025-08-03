#pragma once

#include "config.h"
#include <stdio.h>
#include <stdlib.h>

#define RD__S(X) #X
#define RD_S(X)  RD__S(X)

#define RD_LOG(FMT, ...)                                                                 \
  do {                                                                                   \
    fprintf(stderr, FMT "\n" __VA_OPT__(, ) __VA_ARGS__);                                \
  } while ( false )

#define RD_ASSERT(X)                                                                     \
  if ( !(X) ) {                                                                          \
    RD_LOG(__FILE__ ":" RD_S(__LINE__) ": "                                              \
                                       "assertion failed: " #X);                         \
    __builtin_trap();                                                                    \
    exit(1);                                                                             \
  }

extern struct rd_Config g_config;
