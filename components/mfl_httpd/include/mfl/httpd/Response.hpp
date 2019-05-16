#pragma once

#include <esp_http_server.h>
#include <map>

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

template<typename T>
std::string serializeResponse(const T& response);

// ---------------------------------------------------------------------------------------------------------------------

using Params = std::map<std::string, std::string>;

// ---------------------------------------------------------------------------------------------------------------------

struct Response {
    template<typename T>
    void set(const T& value) {
        body = serializeResponse(value);
    }

    std::string returnType;
    std::string body;
};

// ---------------------------------------------------------------------------------------------------------------------

}
