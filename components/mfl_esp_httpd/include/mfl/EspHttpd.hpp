#pragma once

#include <mfl/esp_httpd/Defaults.hpp>
#include <mfl/httpd/Router.hpp>

#include <cstdint>
#include <cstddef>
#include <esp_http_server.h>



namespace mfl {

// ---------------------------------------------------------------------------------------------------------------------

struct EspHttpd {
    explicit EspHttpd(
            const httpd::Router& router,
            uint16_t port = esp_httpd::DEFAULT_PORT,
            size_t bufferSize = esp_httpd::DEFAULT_BUFFER_SIZE,
            size_t stackSize = esp_httpd::DEFAULT_STACK_SIZE);
    ~EspHttpd();

    void start();
private:
    static std::string readBody(httpd_req_t* req);
    static esp_err_t genericHandler(httpd_req_t *req);

    const httpd::Router& router_;
    uint16_t port_;
    size_t bufferSize_;
    size_t stackSize_;
    httpd_handle_t handle_;
};

// ---------------------------------------------------------------------------------------------------------------------

}
