#include <mfl/httpd/PathNode.hpp>
#include <esp_log.h>

namespace mfl::httpd {

namespace {

// ---------------------------------------------------------------------------------------------------------------------

const char* tag = "pathnode";

// ---------------------------------------------------------------------------------------------------------------------

}

// ---------------------------------------------------------------------------------------------------------------------

PathNode::PathNode(std::vector<std::string> &&args) : args(args) {

}

// ---------------------------------------------------------------------------------------------------------------------

Handler<std::string>& PathNode::handlerFromMethod(http::Method method) {
    switch (method) {
        case http::Method::post:
            return post;
        case http::Method::get:
            return get;
        case http::Method::put:
            return put;
        case http::Method::del:
            return del;
        default:
            ESP_LOGE(tag, "unsupported http::Method");
            throw http::UnsupportedMethod();

    }
}

// ---------------------------------------------------------------------------------------------------------------------

const Handler<std::string> &PathNode::handlerFromMethod(http::Method method) const {
    switch (method) {
        case http::Method::post:
            return post;
        case http::Method::get:
            return get;
        case http::Method::put:
            return put;
        case http::Method::del:
            return del;
        default:
            ESP_LOGE(tag, "unsopported http::Method");
            throw http::UnsupportedMethod();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool PathNode::hasHandler(http::Method method) const {
    return handlerFromMethod(method).operator bool();
}

// ---------------------------------------------------------------------------------------------------------------------

void PathNode::setHandler(http::Method method, const Handler<std::string> &handler) {
    handlerFromMethod(method) = handler;
}

// ---------------------------------------------------------------------------------------------------------------------

}
