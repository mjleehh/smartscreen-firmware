#include <mfl/Httpd.hpp>

#include <mfl/helpers/Defer.hpp>

#include "helpers.hpp"

#include <iostream>
#include <esp_log.h>

namespace mfl {

namespace {

// ---------------------------------------------------------------------------------------------------------------------

const char tag[] = "mfl_httpd";

Httpd* activeServer = nullptr;

bool matchAny(const char *reference_uri, const char *uri_to_match, size_t match_upto) {
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

} // namespace anonymous

// ---------------------------------------------------------------------------------------------------------------------

Httpd::Httpd(uint16_t port, size_t bufferSize)
    : port_(port), bufferSize_(bufferSize)
{

}

// ---------------------------------------------------------------------------------------------------------------------

Httpd::~Httpd() {
    if (activeServer != nullptr) {
        httpd_stop(activeServer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

esp_err_t Httpd::start() {
    if (activeServer != nullptr) {
        ESP_LOGE(tag, "http server already active");
        return ESP_FAIL;
    }
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = port_;
    config.uri_match_fn = matchAny;

    auto err =  httpd_start(&handle_, &config);
    if (err != ESP_OK) {
        ESP_LOGE(tag, "failed to start http server task");
        return err;
    }

    MFL_HELPERS_DEFER({
        if (err != ESP_OK) {
           httpd_stop(&handle_);
           activeServer = nullptr;
        }
    });

    httpd_uri_t postRequest = {
            .uri      = "*",
            .method   = HTTP_POST,
            .handler  = genericHandler,
            .user_ctx = nullptr
    };
    err = httpd_register_uri_handler(handle_, &postRequest);
    if (err != ESP_OK) {
        ESP_LOGE(tag, "failed to register post handler");
        return err;
    }

    httpd_uri_t getRequest = {
            .uri      = "*",
            .method   = HTTP_GET,
            .handler  = genericHandler,
            .user_ctx = nullptr
    };
    httpd_register_uri_handler(handle_, &getRequest);

    httpd_uri_t putRequest = {
            .uri      = "*",
            .method   = HTTP_PUT,
            .handler  = genericHandler,
            .user_ctx = nullptr
    };
    httpd_register_uri_handler(handle_, &putRequest);

    httpd_uri_t deleteRequest = {
            .uri      = "*",
            .method   = HTTP_DELETE,
            .handler  = genericHandler,
            .user_ctx = nullptr
    };
    httpd_register_uri_handler(handle_, &deleteRequest);

    activeServer = this;
    return err;
}

esp_err_t Httpd::get(const std::string& uriTemplate, const Httpd::Handler& handler) {
    auto uri = httpd::splitUrl(uriTemplate);

    root_.get = handler;

//    // edge case root
//    if (uri.empty()) {
//        if (root_.get) {
//            throw httpd::EndpointError("handler for endpoint already defined");
//        }
//        root_.get = handler;
//    }

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

void Httpd::invokeHandler(const PathNode& node, httpd_req_t* req) {
    auto bufferSize = std::max(activeServer->bufferSize_, req->content_len);
    auto maxDataLengh = bufferSize - 1;
    std::string buffer(bufferSize, '\0');
    auto bytesRead = httpd_req_recv(req, &buffer[0], maxDataLengh);
    if (bytesRead < 0) {
        // client closed the connection
        return;
    }
    buffer[bytesRead] = '\0';

    switch (req->method) {
        case HTTP_POST:
            node.post(buffer);
            break;
        case HTTP_GET:
            node.get(buffer);
            break;
        case HTTP_PUT:
            node.put(buffer);
            break;
        case HTTP_DELETE:
            node.del(buffer);
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

esp_err_t Httpd::genericHandler(httpd_req_t *req) {
    auto uri = httpd::splitUrl(req->uri);
    auto& root = activeServer->root_;

    // edge case root
    if (uri.empty()) {
        invokeHandler(root, req);
    }
    auto res =  "URI POST Response";
    httpd_resp_send(req, res, strlen(res));
    return ESP_OK;
}

// ---------------------------------------------------------------------------------------------------------------------

} // namespace mfl
