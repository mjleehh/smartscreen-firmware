#pragma once

#include <esp_http_server.h>
#include <map>

namespace mfl {

namespace  httpd {

// ---------------------------------------------------------------------------------------------------------------------

using Params = std::map<std::string, std::string>;

// ---------------------------------------------------------------------------------------------------------------------

struct Response {
    std::string body;
};

// ---------------------------------------------------------------------------------------------------------------------

struct Context {
    Context(Params&& params, std::string&& body, httpd_req_t* handle)
        : params(params), body(body), handle(handle)
    {}

    Params params;
    std::string body;
    httpd_req_t *handle;
    Response res;
};

// ---------------------------------------------------------------------------------------------------------------------

} // namespace httpd

} // namespace mfl
