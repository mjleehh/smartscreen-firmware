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

bool isArg(const std::string& segment) {
    return segment[0] == ':';
}

// ---------------------------------------------------------------------------------------------------------------------

std::string argName(const std::string& segment) {
    return segment.substr(1);
}

// ---------------------------------------------------------------------------------------------------------------------

} // namespace anonymous

// =====================================================================================================================

Httpd::Httpd(uint16_t port, size_t bufferSize, size_t stackSize)
    : root_(std::vector<std::string>()), port_(port), bufferSize_(bufferSize), stackSize_(stackSize)
{

}

// ---------------------------------------------------------------------------------------------------------------------

Httpd::~Httpd() {
    if (activeServer != nullptr) {
        httpd_stop(activeServer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

esp_err_t Httpd::post(const std::string &uriTemplate, const httpd::Handler &handler) {
    return addHandler(HTTP_POST, uriTemplate, handler);
}

// ---------------------------------------------------------------------------------------------------------------------

esp_err_t Httpd::get(const std::string &uriTemplate, const httpd::Handler &handler) {
    return addHandler(HTTP_GET, uriTemplate, handler);
}

// ---------------------------------------------------------------------------------------------------------------------

esp_err_t Httpd::put(const std::string &uriTemplate, const httpd::Handler &handler) {
    return addHandler(HTTP_PUT, uriTemplate, handler);
}

// ---------------------------------------------------------------------------------------------------------------------

esp_err_t Httpd::del(const std::string &uriTemplate, const httpd::Handler &handler) {
    return addHandler(HTTP_DELETE, uriTemplate, handler);
}

// ---------------------------------------------------------------------------------------------------------------------

esp_err_t Httpd::start() {
    if (activeServer != nullptr) {
        ESP_LOGE(tag, "http server already active");
        return ESP_FAIL;
    }
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    config.stack_size = stackSize_;
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

// ---------------------------------------------------------------------------------------------------------------------

esp_err_t Httpd::addHandler(httpd_method_t method, const std::string& uriTemplate, const httpd::Handler& handler) {
    auto uri = httpd::splitUrl(uriTemplate);

    auto *node = &root_;
    auto uriPos = uri.begin();

    while (uriPos != uri.end()) {
        auto &segment = *uriPos;
        if (segment[0] == ':') {
            ESP_LOGE(tag, "URI arg with no node %s", uriTemplate.c_str());
            return ESP_FAIL;
        }

        std::vector<std::string> args;
        ++uriPos;
        while (uriPos != uri.end() && isArg(*uriPos)) {
            args.push_back(argName(*uriPos));
            ++uriPos;
        }

        auto &children = node->children;
        auto child = children.find(segment);
        if (child != children.end()) {
            if (args.size() != child->second.args.size()) {
                ESP_LOGE(tag, "different argument number defined for existing node %s", uriTemplate.c_str());
                return ESP_FAIL;
            }
            node = &child->second;
        } else {
            node = &children.emplace(segment, std::move(args)).first->second;
        }
    }

    if (node->hasHandler(method)) {
        ESP_LOGE(tag, "handler already defined for node %s", uriTemplate.c_str());
        return ESP_FAIL;
    }
    node->setHandler(method, handler);
    ESP_LOGI(tag, "added URI handler for %s", uriTemplate.c_str());
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string Httpd::readBody(httpd_req_t* req) {
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

esp_err_t Httpd::genericHandler(httpd_req_t *req) {
    auto uri = httpd::splitUrl(req->uri);
    const auto* node = &activeServer->root_;
    std::map<std::string, std::string> argValues;
    const httpd::PathNode* handlerNode = nullptr;

    MFL_HELPERS_DEFER({
        auto method = static_cast<httpd_method_t>(req->method);
        if (handlerNode && handlerNode->hasHandler(method)) {
            httpd::Context context(std::move(argValues), readBody(req), req);
            handlerNode->handlerFromMethod(method)(context);
            httpd_resp_send(req, context.res.body.data(), context.res.body.size());
        } else {
            std::string empty = "";
            httpd_resp_send(req, empty.data(), empty.size());
            httpd_resp_set_status(req, HTTPD_404);
        }
    });

    // edge case root
    if (uri.empty()) {
        handlerNode = node;
        return ESP_OK;
    }

    try {
        auto uriPos = uri.begin();

        // traverse the tree until, the node described by the URI is encountered
        while (uriPos != uri.end()) {
            node = &node->children.at(*uriPos);
            const auto& args = node->args;
            auto argPos = args.begin();
            ++uriPos;
            while (argPos != args.end()) {
                if (uriPos == uri.end()) {
                    ESP_LOGI(tag, "URI path incomplete %s", req->uri);
                    return ESP_OK;
                }
                argValues[*argPos] = *uriPos;
                ++argPos;
                ++uriPos;
            }
        }
        handlerNode = node;

    } catch(const std::out_of_range&) {
        ESP_LOGI(tag, "URI path does not exist %s", req->uri);
    }

    return ESP_OK;
}

// ---------------------------------------------------------------------------------------------------------------------

} // namespace mfl
