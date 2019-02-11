#ifndef POOL_UTILS_H_
#define POOL_UTILS_H_

#include <string>
#include <sstream>
#include <vector>

//#include <utilstrencodings.h>

#include <libconfig.h++>

using libconfig::Setting;
using namespace std;

static size_t
CurlWriteChunkCallback(void *contents, size_t size, size_t nmemb, void *userp);

bool httpGET (const char *url, string &response, long timeoutMs);
bool httpGET (const char *url, const char *userpwd,
              string &response, long timeoutMs);
bool httpPOST(const char *url, const char *userpwd, const char *postData,
              string &response, long timeoutMs, const char *contentType);
bool httpPOST(const char *url, const char *userpwd, const char *postData,
              string &response, long timeoutMs, const char *contentType, const char *agent);
bool blockchainNodeRpcCall(const char *url, const char *userpwd, const char *reqData,
                     string &response);

bool rpcCall(const char *url, const char *userpwd, const char *reqData, int len, string &response, const char *agent);  

#endif
