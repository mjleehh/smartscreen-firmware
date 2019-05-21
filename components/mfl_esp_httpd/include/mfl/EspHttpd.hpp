#pragma once

#include <mfl/esp_httpd/Defaults.hpp>
#include <mfl/httpd/Router.hpp>
#include <esp_http_server.h>



namespace mfl {

// ---------------------------------------------------------------------------------------------------------------------

/**
 * HTTP server for ESP32
 *
 * C++ rapper for the esp_http_server library.
 *
 */
struct EspHttpd {
    /**
     * Create a new Httpd Instance.
     *
     * NOTE: This class is a singleton. It should be placed on the stack for initialization order.
     *
     * @param router the router instance to be used
     * @param port the port the server serves
     * @param bufferSize the maximum request body size
     * @param stackSize the stack size of the server task
     */
    explicit EspHttpd(
            const httpd::Router& router,
            uint16_t port = esp_httpd::DEFAULT_PORT,
            size_t bufferSize = esp_httpd::DEFAULT_BUFFER_SIZE,
            size_t stackSize = esp_httpd::DEFAULT_STACK_SIZE);
    ~EspHttpd();

    /**
     * Starts the server.
     *
     * This method is non blocking. Server will be spawned in seperate task.
     */
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
