#include <mfl/EspHttpd.hpp>
#include <mfl/httpd/Errors.hpp>
#include <mfl/helpers/Defer.hpp>

#include <esp_log.h>

namespace mfl {

namespace {

// ---------------------------------------------------------------------------------------------------------------------

const char tag[] = "mfl_esp_httpd";

// ---------------------------------------------------------------------------------------------------------------------

EspHttpd* activeServer = nullptr;

// ---------------------------------------------------------------------------------------------------------------------

bool matchAny(const char *reference_uri, const char *uri_to_match, size_t match_upto) {
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

const char* responseCodeStrings(httpd::Status response) {
    using namespace httpd;
    switch (response) {
        case Status::ok:
            return HTTPD_200;
        case Status::badRequest:
            return HTTPD_400;
        case Status::notFound:
            return HTTPD_404;
        case Status::internalServerError:
            return HTTPD_500;
    }
    // FIXME: useless, but compiler issues incorrect warning
    return HTTPD_500;
}

http::Method methodFromRequest(int method) {
    using namespace httpd;
    switch (method) {
        case HTTP_POST:
            return http::Method::post;
        case HTTP_GET:
            return http::Method::get;
        case HTTP_PUT:
            return http::Method::put;
        case HTTP_DELETE:
            return http::Method::del;
        default:
            throw http::UnsupportedMethod();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}

// ---------------------------------------------------------------------------------------------------------------------

EspHttpd::EspHttpd(const httpd::Router& router, uint16_t port, size_t bufferSize, size_t stackSize)
    : router_(router), port_(port), bufferSize_(bufferSize), stackSize_(stackSize), handle_(nullptr)
{

}

// ---------------------------------------------------------------------------------------------------------------------

EspHttpd::~EspHttpd() {
    if (activeServer != nullptr) {
        httpd_stop(activeServer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void EspHttpd::start() {
    if (activeServer != nullptr) {
        throw httpd::ServerError("httpd already active");
    }
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    config.stack_size = stackSize_;
    config.server_port = port_;
    config.uri_match_fn = matchAny;

    auto err =  httpd_start(&handle_, &config);
    if (err != ESP_OK) {
        throw httpd::ServerError("failed to start httpd server task");
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
        throw httpd::EndpointError("failed to register post handler");
    }

    httpd_uri_t getRequest = {
            .uri      = "*",
            .method   = HTTP_GET,
            .handler  = genericHandler,
            .user_ctx = nullptr
    };
    err = httpd_register_uri_handler(handle_, &getRequest);
    if (err != ESP_OK) {
        throw httpd::EndpointError("failed to register get handler");
    }

    httpd_uri_t putRequest = {
            .uri      = "*",
            .method   = HTTP_PUT,
            .handler  = genericHandler,
            .user_ctx = nullptr
    };
    err = httpd_register_uri_handler(handle_, &putRequest);
    if (err != ESP_OK) {
        throw httpd::EndpointError("failed to register put handler");
    }


    httpd_uri_t deleteRequest = {
            .uri      = "*",
            .method   = HTTP_DELETE,
            .handler  = genericHandler,
            .user_ctx = nullptr
    };
    err = httpd_register_uri_handler(handle_, &deleteRequest);
    if (err != ESP_OK) {
        throw httpd::EndpointError("failed to register delete handler");
    }

    activeServer = this;
}

// ---------------------------------------------------------------------------------------------------------------------

esp_err_t EspHttpd::genericHandler(httpd_req_t *req) {
    try {
        httpd::Context<std::string> context;
        context.uri = req->uri;
        context.body = readBody(req);
        context.method = methodFromRequest(req->method);

        if (activeServer != nullptr) {
            activeServer->router_.handle(context);
        }

        httpd_resp_set_status(req, responseCodeStrings(context.res.status));
        const auto& res = context.res;
        httpd_resp_send(req, res.body.data(), res.body.length());
        return ESP_OK;
    } catch (const std::exception& e) {
        ESP_LOGE(tag, "error handling endpoing: %s", e.what());
        return ESP_FAIL;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::string EspHttpd::readBody(httpd_req_t* req) {
    auto bufferSize = std::max(activeServer->bufferSize_, req->content_len);
    auto maxDataLengh = bufferSize - 1;
    std::string buffer(bufferSize, '\0');
    auto bytesRead = httpd_req_recv(req, &buffer[0], maxDataLengh);
    if (bytesRead < 0) {
        // client closed the connection
        return ESP_OK;
    }
    buffer[bytesRead] = '\0';
    return buffer;
}

// ---------------------------------------------------------------------------------------------------------------------

}
