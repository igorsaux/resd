#include "disk.h"
#include "app.h"
#include <errno.h>
#include <string.h>
#include <sys/statfs.h>
#include <sys/vfs.h>

[[nodiscard]] bool rd_DiskStat_get(struct rd_DiskStat* self, const char* mount_point)
{
  RD_ASSERT(self != nullptr);
  RD_ASSERT(mount_point != nullptr);

  struct statfs statfs_info = {};

  if ( statfs("/", &statfs_info) == -1 ) {
    RD_LOG("failed to get statfs for '/': %s", strerror(errno));

    return false;
  }

  const auto total_bytes = statfs_info.f_blocks * statfs_info.f_bsize;
  const auto free_bytes  = statfs_info.f_bfree * statfs_info.f_bsize;

  self->total_gigs    = (float)total_bytes / 1024.0f / 1024.0f / 1024.0f;
  self->free_gigs     = (float)free_bytes / 1024.0f / 1024.0f / 1024.0f;
  self->usage_percent = 100.0f - (self->free_gigs / self->total_gigs * 100.0f);

  return true;
}
