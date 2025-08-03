#pragma once

#include <stddef.h>
#include <stdint.h>

struct rd_Config {
  const char* discord_webhook;
  uint64_t    discord_mention_user;
  float       max_disk_usage;
};
