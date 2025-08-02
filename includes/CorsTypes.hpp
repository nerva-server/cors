#pragma once

#include <string>
#include <unordered_set>
#include <vector>

using Origin = std::string;
using Origins = std::unordered_set<Origin>;
using HttpMethod = std::string;
using HttpMethods = std::unordered_set<HttpMethod>;
using HttpHeader = std::string;
using HttpHeaders = std::unordered_set<HttpHeader>;
using Credentials = bool;
using MaxAge = int;

struct CorsConfig
{
    Origins origins;
    HttpMethods methods;
    HttpHeaders allowHeaders;
    HttpHeaders exposeHeaders;
    Credentials credentials;
    MaxAge maxAge;

    CorsConfig() : credentials(false), maxAge(86400)
    {
        methods = {"GET", "POST", "PUT", "DELETE", "OPTIONS", "PATCH"};

        allowHeaders = {"Content-Type", "Authorization", "Accept", "Origin"};
        exposeHeaders = {"Content-Length", "Content-Type"};
    }
};

enum class CorsPolicy
{
    ALLOW_ALL,
    ALLOW_SPECIFIC,
    ALLOW_SAME_ORIGIN,
    BLOCK_ALL
};