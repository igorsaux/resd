#include "discord.h"
#include "app.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/header.h>
#include <string.h>

static __attribute__((constructor)) void init()
{
  RD_LOG("libcurl init");

  curl_global_init(CURL_GLOBAL_ALL);
}

static __attribute((destructor)) void cleanup()
{
  RD_LOG("libcurl cleanup");

  curl_global_cleanup();
}

struct rd_ResponseBody {
  char*  data;
  size_t pos;
  size_t size;
};

static size_t writeResponseCallback(
    char* content, const size_t size, const size_t nmemb, void* userdata)
{
  const auto response     = (struct rd_ResponseBody*)userdata;
  const auto content_size = size * nmemb;

  if ( response->size - response->pos < content_size ) {
    response->data = realloc(response->data, response->size + content_size);

    RD_ASSERT(response->data != nullptr);
  }

  memcpy(response->data + response->pos, content, content_size);
  response->pos += content_size;

  return content_size;
}

bool rd_Discord_sendWebhook(const char* url, const char* body)
{
  RD_ASSERT(url != nullptr);
  RD_ASSERT(body != nullptr);

  CURL* curl;

  curl = curl_easy_init();

  if ( curl == nullptr ) {
    RD_LOG("failed to get a curl handle!");

    return false;
  }

  struct curl_slist* headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/json");

  struct rd_ResponseBody response = { .data = calloc(1024, 1), .size = 1024 };
  RD_ASSERT(response.data != nullptr);

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeResponseCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

  const auto ret = curl_easy_perform(curl);

  curl_slist_free_all(headers);
  headers = nullptr;

  bool success = true;

  if ( ret != CURLE_OK ) {
    RD_LOG("failed to send HTTP POST request to '%s': %s", url, curl_easy_strerror(ret));
    success = false;

    goto exit;
  }

  long status = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);

  if ( status != 200 && status != 204 ) {
    RD_LOG("response failed with code %ld: %s", status, response.data);
    success = false;

    goto exit;
  }

exit:
  free(response.data);
  curl_easy_cleanup(curl);

  return success;
}
