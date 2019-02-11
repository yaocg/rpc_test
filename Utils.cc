#include "Utils.h"

#include <stdarg.h>
#include <sys/stat.h>

#include <curl/curl.h>
#include <glog/logging.h>

struct CurlChunk {
  char *memory;
  size_t size;
};

static size_t
CurlWriteChunkCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct CurlChunk *mem = (struct CurlChunk *)userp;

  mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

bool httpGET(const char *url, string &response, long timeoutMs) {
  return httpPOST(url, nullptr, nullptr, response, timeoutMs, nullptr);
}

bool httpGET(const char *url, const char *userpwd,
             string &response, long timeoutMs) {
  return httpPOST(url, userpwd, nullptr, response, timeoutMs, nullptr);
}

bool httpPOSTImpl(const char *url, const char *userpwd, const char *postData, int len,
              string &response, long timeoutMs, const char *mineType, const char *agent) {
  struct curl_slist *headers = NULL;
  CURLcode status;
  long code;
  CURL *curl = curl_easy_init();
  struct CurlChunk chunk;
  if (!curl) {
    return false;
  }

  chunk.memory = (char *)malloc(1);  /* will be grown as needed by the realloc above */
  chunk.size   = 0;          /* no data at this point */

  // RSK doesn't support 'Expect: 100-Continue' in 'HTTP/1.1'.
  // So switch to 'HTTP/1.0'.
  curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);

  if (mineType != nullptr) {
    string mineHeader = string("Content-Type: ") + string(mineType);
    headers = curl_slist_append(headers, mineHeader.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  }

  curl_easy_setopt(curl, CURLOPT_URL, url);

  if (postData != nullptr) {
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, len);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,    postData);
  }

  if (userpwd != nullptr)
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd);

  curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, agent);

  curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeoutMs);

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteChunkCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA,     (void *)&chunk);

  status = curl_easy_perform(curl);
  if (status != 0) {
    LOG(ERROR) << "unable to request data from: " << url << ", error: " << curl_easy_strerror(status);
    goto error;
  }

  if (chunk.size > 0)
    response.assign(chunk.memory, chunk.size);

  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
  //status code 200 - 208 indicates ok
  //sia returns 204 as success
  if (code < 200 || code > 208) {
    LOG(ERROR) << "server responded with code: " << code;
    goto error;
  }

  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);
  free(chunk.memory);
  return true;


error:
  if (curl)
    curl_easy_cleanup(curl);
  if (headers)
    curl_slist_free_all(headers);

  free(chunk.memory);
  return false;
}

bool httpPOST(const char *url, const char *userpwd, const char *postData,
              string &response, long timeoutMs, const char *mineType, const char *agent)
{
  return httpPOSTImpl(url, userpwd, postData, postData != nullptr ? strlen(postData) : 0, response, timeoutMs, mineType, agent);
}

bool httpPOST(const char *url, const char *userpwd, const char *postData,
              string &response, long timeoutMs, const char *mineType)
{
  return httpPOST(url, userpwd, postData, response, timeoutMs, mineType, "curl");
}

bool blockchainNodeRpcCall(const char *url, const char *userpwd, const char *reqData,
                     string &response) {
  return httpPOST(url, userpwd, reqData, response, 5000/* timeout ms */, "application/json");
}

bool rpcCall(const char *url, const char *userpwd, const char *reqData, int len, string &response, const char *agent) 
{
  return httpPOSTImpl(url, userpwd, reqData, len, response, 5000, "application/json", agent);
}

