#pragma once

#include <mfl/helpers/macros.hpp>

#include <lwip/ip_addr.h>
#include <esp_event.h>
#include <esp_err.h>

#include <map>
#include <functional>
#include <vector>
#include <esp_http_server.h>

#define MFL_HTTPD_HANDLER_1(code) [](const std::string& body) code
#define MFL_HTTPD_HANDLER_2(capture, code) [capture](const std::string& body) code
#define MFL_HTTPD_HANDLER(...) \
    MFL_HELPERS_GET_ARG_3(__VA_ARGS__, MFL_HTTPD_HANDLER_2, MFL_HTTPD_HANDLER_1)(__VA_ARGS__)

namespace mfl {

namespace  httpd {

const uint16_t DEFAULT_PORT = 80;
const size_t DEFAULT_BUFFER_SIZE = 10 * 1024;

// ---------------------------------------------------------------------------------------------------------------------

struct EndpointError : std::invalid_argument {
    explicit EndpointError(const std::string& what) : invalid_argument(what) {};
};

// ---------------------------------------------------------------------------------------------------------------------

}

// ---------------------------------------------------------------------------------------------------------------------

struct Httpd {
    using Params = std::map<std::string, std::string>;
    using Handler = std::function<void(const std::string& body)>;

    Httpd(uint16_t port = httpd::DEFAULT_PORT, size_t bufferSize = httpd::DEFAULT_BUFFER_SIZE);
    ~Httpd();

    esp_err_t get(const std::string& uriTemplate, const Handler& handler);

    esp_err_t start();

private:
    struct PathNode {
        Handler post;
        Handler get;
        Handler put;
        Handler del;
        std::vector<std::string> args;
        std::map<std::string, PathNode> children;
    };

    static void invokeHandler(const PathNode& node, httpd_req_t* req);
    static esp_err_t genericHandler(httpd_req_t *req);

    PathNode root_;
    uint16_t port_;
    size_t bufferSize_;
    httpd_handle_t handle_;
};

// ---------------------------------------------------------------------------------------------------------------------

} // namespace mfl
