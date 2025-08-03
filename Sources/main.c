#include "app.h"
#include "discord.h"
#include "disk.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static void check_disk_usage()
{
  struct rd_DiskStat disk_stat;

  if ( !rd_DiskStat_get(&disk_stat, "/") ) {
    RD_LOG("failed to check disk usage");

    return;
  }

  if ( disk_stat.usage_percent <= g_config.max_disk_usage ) {
    RD_LOG("the disk space is enough");

    return;
  }

  RD_LOG("disk usage is high! (%.1f%%)", disk_stat.usage_percent);

  if ( g_config.discord_webhook == nullptr )
    return;

  RD_LOG("sending a high disk usage alert to discord");

  char content_message[64] = { '\0' };

  (void)snprintf(
      content_message, sizeof(content_message), "<@%lu>", g_config.discord_mention_user);

  const char* body_template
      = "{\"content\":\"%s\",\"embeds\":[{"
        "\"title\":\"💾 Disk Alarm\",\"description\":\"The disk space is low!\","
        "\"color\":15405075,\"fields\":[{\"name\":\"Total\",\"value\":\"%.2fG\","
        "\"inline\":true},{\"name\":\"Free\",\"value\":\"%.2fG\",\"inline\":true},{"
        "\"name\":\"Usage\",\"value\":\"%.1f%%\",\"inline\":true}]}],\"attachments\":[],"
        "\"allowed_mentions\":{\"parse\":[\"users\"],\"users\":[]},\"username\":"
        "\"Host\"}";

  char body[1024] = { '\0' };

  snprintf(body, sizeof(body), body_template, content_message, disk_stat.total_gigs,
      disk_stat.free_gigs, disk_stat.usage_percent);

  rd_Discord_sendWebhook(g_config.discord_webhook, body);
}

int main(const int argc, const char* argv[])
{
  RD_LOG(
      "resd - an utility for checking server resources and sending a message about the "
      "need for maintenance.");

  check_disk_usage();

  return 0;
}
