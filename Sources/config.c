#include "config.h"
#include "app.h"
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

static char gp_config_file[] = {
#embed "../config.ini" if_empty('\n')
  , '\0'
};

bool rd_Config_trySet(struct rd_Config* self, const char* key, const char* value)
{
  RD_ASSERT(self != nullptr);

  if ( strcmp(key, "discord_webhook") == 0 ) {
    self->discord_webhook = value;

    return true;
  } else if ( strcmp(key, "max_disk_usage") == 0 ) {
    const auto v = strtof(value, nullptr);

    if ( errno == ERANGE ) {
      RD_LOG("failed to parse key '%s': value '%s' is out of range", key, value);

      return false;
    } else if ( v == 0 ) {
      RD_LOG("failed to parse key '%s': value '%s' is not a float number", key, value);

      return false;
    }

    self->max_disk_usage = v;

    return true;
  } else if ( strcmp(key, "discord_mention_user") == 0 ) {
    if ( strcmp(value, "0") == 0 ) {
      self->discord_mention_user = 0;

      return true;
    }

    const auto v = strtoull(value, nullptr, 10);

    if ( errno == ERANGE ) {
      RD_LOG("failed to parse key '%s': value '%s' is out of range", key, value);

      return false;
    } else if ( v == 0 ) {
      RD_LOG("failed to parse key '%s': value '%s' is not a number", key, value);

      return false;
    }

    self->discord_mention_user = v;

    return true;
  }

  return false;
}

void rd_Config_dump(const struct rd_Config self)
{
  RD_LOG("--- CONFIG ---");

  if ( self.discord_webhook == nullptr )
    RD_LOG("* discord_webhook = nullptr");
  else
    RD_LOG("* discord_webhook = %s", self.discord_webhook);

  RD_LOG("* discord_mention_user = %lu", self.discord_mention_user);
  RD_LOG("* max_disk_usage = %.1f%%", self.max_disk_usage);
}

void rd_Config_parse(struct rd_Config* self)
{
  RD_ASSERT(self != nullptr);
  RD_LOG("parsing config");

  size_t pos       = 0;
  bool   skip_line = false;

  while ( true ) {
    char c = gp_config_file[pos];

    if ( c == '\0' )
      break;

    if ( c == '\n' ) {
      skip_line = false;
      pos += 1;

      continue;
    }

    if ( skip_line || isspace(c) ) {
      pos += 1;

      continue;
    }

    if ( c == ';' ) {
      skip_line = true;
      pos += 1;

      continue;
    }

    const auto key = gp_config_file + pos;
    const auto sep = strchr(key, '=');

    if ( sep == nullptr ) {
      auto end = strchr(key, '\n');

      if ( end == nullptr )
        end = strchr(key, '\0');

      RD_ASSERT(end != nullptr);

      *end = '\0';

      RD_LOG("missing value for key '%s'", key);

      skip_line = true;

      while ( gp_config_file + pos != end )
        pos += 1;

      pos += 1;

      continue;
    }

    auto end = strchr(sep, '\n');

    if ( end == nullptr )
      end = strchr(sep, '\0');

    RD_ASSERT(end != nullptr);

    const auto value = sep + 1;

    *end = '\0';
    *sep = '\0';

    if ( !rd_Config_trySet(self, key, value) ) {
      RD_LOG("unknown key '%s'", key);
    }

    while ( gp_config_file + pos != end )
      pos += 1;

    pos += 1;
  }

  RD_LOG("config parsed");
}
