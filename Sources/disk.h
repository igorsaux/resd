#pragma once

#include <stddef.h>
#include <stdint.h>

struct rd_DiskStat {
  float total_gigs;
  float free_gigs;
  float  usage_percent;
};

[[nodiscard]] bool rd_DiskStat_get(struct rd_DiskStat* self, const char* mount_point);
