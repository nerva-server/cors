#pragma once

#include <nerva/IHandler.hpp>
#include "CorsTypes.hpp"

class Cors : public IHandler {
public:
    Cors();
    Cors(const CorsConfig& config);
    Cors(CorsPolicy policy);
    
    void setConfig(const CorsConfig& config);
    void setPolicy(CorsPolicy policy);
    
    void allowOrigin(const std::string& origin);
    void allowOrigins(const std::vector<std::string>& origins);
    void allowAllOrigins();
    void blockAllOrigins();
    void removeOrigin(const std::string& origin);
    void clearOrigins();
    
    void allowMethod(const std::string& method);
    void allowMethods(const std::vector<std::string>& methods);
    void removeMethod(const std::string& method);
    
    void allowHeader(const std::string& header);
    void allowHeaders(const std::vector<std::string>& headers);
    void exposeHeader(const std::string& header);
    void exposeHeaders(const std::vector<std::string>& headers);
    
    void setCredentials(bool enabled);
    void setMaxAge(int seconds);
    
    bool isOriginAllowed(const std::string& origin) const;
    bool isMethodAllowed(const std::string& method) const;
    bool isHeaderAllowed(const std::string& header) const;
    
    void Handle(Http::Request &req, Http::Response &res, std::function<void()> next) override;

private:
    CorsConfig config;
    CorsPolicy policy;
    
    bool shouldAllowRequest(const std::string& origin) const;
    void setCorsHeaders(Http::Response &res, const std::string& origin);
    std::string detectProtocol(const Http::Request &req) const;
    std::string buildOrigin(const Http::Request &req) const;
};