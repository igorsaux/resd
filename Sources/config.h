#pragma once

#include <stddef.h>
#include <stdint.h>

struct rd_Config {
  const char* discord_webhook;
  uint64_t    discord_mention_user;
  float       max_disk_usage;
};

bool rd_Config_trySet(struct rd_Config* self, const char* key, const char* value);

void rd_Config_dump(const struct rd_Config self);

void rd_Config_parse(struct rd_Config* self);
