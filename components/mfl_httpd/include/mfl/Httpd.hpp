#pragma once

#include <mfl/httpd/Context.hpp>
#include <mfl/httpd/PathNode.hpp>
#include <lwip/ip_addr.h>
#include <esp_event.h>
#include <esp_err.h>

namespace mfl {

namespace  httpd {

// ---------------------------------------------------------------------------------------------------------------------

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
    explicit Httpd(uint16_t port = httpd::DEFAULT_PORT, size_t bufferSize = httpd::DEFAULT_BUFFER_SIZE);
    ~Httpd();

    esp_err_t post(const std::string& uriTemplate, const httpd::Handler& handler);
    esp_err_t get(const std::string& uriTemplate, const httpd::Handler& handler);
    esp_err_t put(const std::string& uriTemplate, const httpd::Handler& handler);
    esp_err_t del(const std::string& uriTemplate, const httpd::Handler& handler);

    esp_err_t start();

private:
    static std::string readBody(httpd_req_t* req);
    static esp_err_t genericHandler(httpd_req_t *req);
    esp_err_t addHandler(httpd_method_t method, const std::string& uriTemplate, const httpd::Handler& handler);

    httpd::PathNode root_;
    uint16_t port_;
    size_t bufferSize_;
    httpd_handle_t handle_;
};

// ---------------------------------------------------------------------------------------------------------------------

} // namespace mfl
