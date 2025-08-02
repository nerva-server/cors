#include "Cors.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>

Cors::Cors() : policy(CorsPolicy::ALLOW_ALL) {
    allowAllOrigins();
}

Cors::Cors(const CorsConfig& config) : config(config), policy(CorsPolicy::ALLOW_SPECIFIC) {
}

Cors::Cors(CorsPolicy policy) : policy(policy) {
    switch (policy) {
        case CorsPolicy::ALLOW_ALL:
            allowAllOrigins();
            break;
        case CorsPolicy::ALLOW_SAME_ORIGIN:
            break;
        case CorsPolicy::BLOCK_ALL:
            break;
    }
}

void Cors::setConfig(const CorsConfig& newConfig) {
    config = newConfig;
}

void Cors::setPolicy(CorsPolicy newPolicy) {
    policy = newPolicy;
    if (policy == CorsPolicy::ALLOW_ALL) {
        allowAllOrigins();
    }
}

void Cors::allowOrigin(const std::string& origin) {
    config.origins.insert(origin);
}

void Cors::allowOrigins(const std::vector<std::string>& origins) {
    for (const auto& origin : origins) {
        config.origins.insert(origin);
    }
}

void Cors::allowAllOrigins() {
    config.origins.clear();
    config.origins.insert("*");
}

void Cors::blockAllOrigins() {
    config.origins.clear();
}

void Cors::removeOrigin(const std::string& origin) {
    config.origins.erase(origin);
}

void Cors::clearOrigins() {
    config.origins.clear();
}

void Cors::allowMethod(const std::string& method) {
    config.methods.insert(method);
}

void Cors::allowMethods(const std::vector<std::string>& methods) {
    for (const auto& method : methods) {
        config.methods.insert(method);
    }
}

void Cors::removeMethod(const std::string& method) {
    config.methods.erase(method);
}

void Cors::allowHeader(const std::string& header) {
    config.allowHeaders.insert(header);
}

void Cors::allowHeaders(const std::vector<std::string>& headers) {
    for (const auto& header : headers) {
        config.allowHeaders.insert(header);
    }
}

void Cors::exposeHeader(const std::string& header) {
    config.exposeHeaders.insert(header);
}

void Cors::exposeHeaders(const std::vector<std::string>& headers) {
    for (const auto& header : headers) {
        config.exposeHeaders.insert(header);
    }
}

void Cors::setCredentials(bool enabled) {
    config.credentials = enabled;
}

void Cors::setMaxAge(int seconds) {
    config.maxAge = seconds;
}

bool Cors::isOriginAllowed(const std::string& origin) const {
    if (origin.empty()) {
        return true;
    }
    return config.origins.find("*") != config.origins.end() || 
           config.origins.find(origin) != config.origins.end();
}

bool Cors::isMethodAllowed(const std::string& method) const {
    return config.methods.find(method) != config.methods.end();
}

bool Cors::isHeaderAllowed(const std::string& header) const {
    return config.allowHeaders.find(header) != config.allowHeaders.end();
}

std::string Cors::detectProtocol(const Http::Request &req) const {
    if (!req.getHeader("X-Forwarded-Proto").empty()) {
        return req.getHeader("X-Forwarded-Proto");
    }
    if (!req.getHeader("X-Forwarded-Ssl").empty() && req.getHeader("X-Forwarded-Ssl") == "on") {
        return "https";
    }
    if (!req.getHeader("X-Forwarded-Port").empty() && req.getHeader("X-Forwarded-Port") == "443") {
        return "https";
    }
    return "http";
}

std::string Cors::buildOrigin(const Http::Request &req) const {
    std::string host = req.getHeader("Host");
    if (host.empty()) return "";
    
    std::string protocol = detectProtocol(req);
    return protocol + "://" + host;
}

bool Cors::shouldAllowRequest(const std::string& origin) const {
    switch (policy) {
        case CorsPolicy::ALLOW_ALL:
            return true;
        case CorsPolicy::ALLOW_SPECIFIC:
            if (origin.empty()) {
                return true;
            }
            return isOriginAllowed(origin);
        case CorsPolicy::ALLOW_SAME_ORIGIN:
            return origin.empty();
        case CorsPolicy::BLOCK_ALL:
            return false;
        default:
            return false;
    }
}

void Cors::setCorsHeaders(Http::Response &res, const std::string& origin) {
    if (config.origins.find("*") != config.origins.end()) {
        res.setHeader("Access-Control-Allow-Origin", "*");
    } else if (origin.empty()) {
    } else {
        res.setHeader("Access-Control-Allow-Origin", origin);
    }
    
    std::ostringstream methodsStream;
    for (const auto& method : config.methods) {
        methodsStream << method << ", ";
    }
    std::string methodsStr = methodsStream.str();
    if (!methodsStr.empty()) {
        methodsStr.pop_back();
        methodsStr.pop_back();
    }
    res.setHeader("Access-Control-Allow-Methods", methodsStr);
    
    std::ostringstream headersStream;
    for (const auto& header : config.allowHeaders) {
        headersStream << header << ", ";
    }
    std::string headersStr = headersStream.str();
    if (!headersStr.empty()) {
        headersStr.pop_back();
        headersStr.pop_back();
    }
    res.setHeader("Access-Control-Allow-Headers", headersStr);
    
    if (config.credentials) {
        res.setHeader("Access-Control-Allow-Credentials", "true");
    }
    
    res.setHeader("Access-Control-Max-Age", std::to_string(config.maxAge));
    
    if (!config.exposeHeaders.empty()) {
        std::ostringstream exposeStream;
        for (const auto& header : config.exposeHeaders) {
            exposeStream << header << ", ";
        }
        std::string exposeStr = exposeStream.str();
        if (!exposeStr.empty()) {
            exposeStr.pop_back();
            exposeStr.pop_back();
        }
        res.setHeader("Access-Control-Expose-Headers", exposeStr);
    }
}

void Cors::Handle(Http::Request &req, Http::Response &res, std::function<void()> next) {
    std::string origin = req.getHeader("Origin");
    
    if (origin.empty()) {
        if (req.method == "OPTIONS") {
            setCorsHeaders(res, "");
            res.statusCode = 204;
            return;
        }
        setCorsHeaders(res, "");
        next();
        return;
    }
    
    if (!shouldAllowRequest(origin)) {
        res << 403 << "CORS: Origin not allowed";
        return;
    }
    
    if (req.method == "OPTIONS") {
        setCorsHeaders(res, origin);
        res.statusCode = 204;
        return;
    }
    
    setCorsHeaders(res, origin);
    next();
}