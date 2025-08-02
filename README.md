# CORS Module for Nerva HTTP Server

This module provides Cross-Origin Resource Sharing (CORS) support for the Nerva HTTP server. It enables web applications to securely manage requests from different domains.

## Features

- ✅ **Flexible Origin Management**: Specific origins, all origins, or same-origin policies
- ✅ **HTTP Method Control**: Configure allowed HTTP methods
- ✅ **Header Management**: Configure allowed and exposed headers
- ✅ **Credentials Support**: Cookie and authentication header support
- ✅ **Max Age Settings**: Preflight cache duration configuration
- ✅ **Nerva HTTP Server Integration**: Full compatibility with IHandler interface

## Installation

### Requirements

- C++20 compatible compiler (clang++ recommended)
- Nerva HTTP Server library
- Make build system

### Build

```bash
make all
```

### Clean

```bash
make clean
```

## Usage

### Basic Usage

```cpp
#include "Cors.hpp"

// Create CORS configuration
CorsConfig config;
config.origins.insert("http://localhost:8080");
config.origins.insert("https://myapp.com");

// Create CORS handler
Cors cors = Cors(config);

// Add to server
server.Use("/*", cors);
```

### Advanced Configuration

```cpp
#include "Cors.hpp"

CorsConfig config;

// Origin settings
config.origins.insert("http://localhost:3000");
config.origins.insert("https://production.com");

// HTTP methods
config.methods.insert("GET");
config.methods.insert("POST");
config.methods.insert("PUT");
config.methods.insert("DELETE");

// Allowed headers
config.allowHeaders.insert("Content-Type");
config.allowHeaders.insert("Authorization");
config.allowHeaders.insert("X-Custom-Header");

// Exposed headers
config.exposeHeaders.insert("X-Total-Count");
config.exposeHeaders.insert("X-Page-Count");

// Credentials and cache settings
config.credentials = true;
config.maxAge = 3600; // 1 hour

Cors cors = Cors(config);
server.Use("/*", cors);
```

### Programmatic Configuration

```cpp
Cors cors;

// Origin management
cors.allowOrigin("http://localhost:8080");
cors.allowOrigins({"https://app1.com", "https://app2.com"});
cors.removeOrigin("http://localhost:3000");
cors.allowAllOrigins(); // Allow all origins
cors.blockAllOrigins(); // Block all origins

// Method management
cors.allowMethod("PATCH");
cors.allowMethods({"GET", "POST", "PUT"});
cors.removeMethod("DELETE");

// Header management
cors.allowHeader("X-API-Key");
cors.allowHeaders({"Content-Type", "Authorization"});
cors.exposeHeader("X-Total-Count");

// Other settings
cors.setCredentials(true);
cors.setMaxAge(7200); // 2 hours

server.Use("/*", cors);
```

### Policies

```cpp
// Allow all origins
Cors cors1 = Cors(CorsPolicy::ALLOW_ALL);

// Allow only specific origins
Cors cors2 = Cors(CorsPolicy::ALLOW_SPECIFIC);

// Allow only same origin
Cors cors3 = Cors(CorsPolicy::ALLOW_SAME_ORIGIN);

// Block all origins
Cors cors4 = Cors(CorsPolicy::BLOCK_ALL);
```

## API Reference

### CorsConfig Struct

```cpp
struct CorsConfig {
    Origins origins;           // Allowed origins
    HttpMethods methods;       // Allowed HTTP methods
    HttpHeaders allowHeaders;  // Allowed headers
    HttpHeaders exposeHeaders; // Exposed headers
    Credentials credentials;   // Credentials support
    MaxAge maxAge;            // Preflight cache duration
};
```

### CorsPolicy Enum

```cpp
enum class CorsPolicy {
    ALLOW_ALL,        // Allow all origins
    ALLOW_SPECIFIC,   // Allow only specific origins
    ALLOW_SAME_ORIGIN, // Allow only same origin
    BLOCK_ALL         // Block all origins
};
```

### Cors Class Methods

#### Constructors
- `Cors()` - Default configuration (allows all origins)
- `Cors(const CorsConfig& config)` - With custom configuration
- `Cors(CorsPolicy policy)` - With policy

#### Origin Management
- `allowOrigin(const std::string& origin)` - Add single origin
- `allowOrigins(const std::vector<std::string>& origins)` - Add multiple origins
- `allowAllOrigins()` - Allow all origins
- `blockAllOrigins()` - Block all origins
- `removeOrigin(const std::string& origin)` - Remove origin
- `clearOrigins()` - Clear all origins

#### Method Management
- `allowMethod(const std::string& method)` - Add single method
- `allowMethods(const std::vector<std::string>& methods)` - Add multiple methods
- `removeMethod(const std::string& method)` - Remove method

#### Header Management
- `allowHeader(const std::string& header)` - Add allowed header
- `allowHeaders(const std::vector<std::string>& headers)` - Add multiple headers
- `exposeHeader(const std::string& header)` - Add exposed header
- `exposeHeaders(const std::vector<std::string>& headers)` - Add multiple exposed headers

#### Other Settings
- `setCredentials(bool enabled)` - Enable/disable credentials support
- `setMaxAge(int seconds)` - Set preflight cache duration

#### Control Methods
- `isOriginAllowed(const std::string& origin)` - Check if origin is allowed
- `isMethodAllowed(const std::string& method)` - Check if method is allowed
- `isHeaderAllowed(const std::string& header)` - Check if header is allowed

## Default Values

```cpp
CorsConfig() {
    credentials = false;
    maxAge = 86400; // 24 hours
    
    methods = {"GET", "POST", "PUT", "DELETE", "OPTIONS", "PATCH"};
    allowHeaders = {"Content-Type", "Authorization", "Accept", "Origin"};
    exposeHeaders = {"Content-Length", "Content-Type"};
}
```

## Example Scenarios

### Development Environment

```cpp
CorsConfig devConfig;
devConfig.origins.insert("http://localhost:3000");
devConfig.origins.insert("http://localhost:8080");
devConfig.credentials = true;

Cors devCors = Cors(devConfig);
server.Use("/*", devCors);
```

### Production Environment

```cpp
CorsConfig prodConfig;
prodConfig.origins.insert("https://myapp.com");
prodConfig.origins.insert("https://www.myapp.com");
prodConfig.credentials = true;
prodConfig.maxAge = 3600;

Cors prodCors = Cors(prodConfig);
server.Use("/*", prodCors);
```

### API Gateway

```cpp
CorsConfig apiConfig;
apiConfig.origins.insert("https://api.myapp.com");
apiConfig.methods = {"GET", "POST", "PUT", "DELETE"};
apiConfig.allowHeaders = {"Content-Type", "Authorization", "X-API-Key"};
apiConfig.exposeHeaders = {"X-Total-Count", "X-Page-Count"};

Cors apiCors = Cors(apiConfig);
server.Use("/api/*", apiCors);
```

## Security Notes

1. **Don't use `*` in production**: Avoid allowing all origins in production for security
2. **Don't use `*` with credentials**: Wildcard origin usage with credentials creates security risks
3. **HTTPS requirement**: Only allow HTTPS origins in production environment
4. **Header filtering**: Only expose necessary headers

## Same-Origin Requests

The CORS module automatically allows requests from the same domain (where the Origin header is empty). This means:
- Requests from the same domain will always pass CORS checks
- No additional configuration is needed for same-origin requests
- This behavior applies to all CORS policies except `BLOCK_ALL`

## License

This module is part of the Nerva HTTP Server project. 