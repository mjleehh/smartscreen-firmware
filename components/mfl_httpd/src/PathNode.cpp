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

Handler<std::string>& PathNode::handlerFromMethod(Method method) {
    switch (method) {
        case Method::post:
            return post;
        case Method::get:
            return get;
        case Method::put:
            return put;
        case Method::del:
            return del;
        default:
            ESP_LOGE(tag, "unsupported method");
            throw UnsupportedMethod();

    }
}

// ---------------------------------------------------------------------------------------------------------------------

const Handler<std::string> &PathNode::handlerFromMethod(Method method) const {
    switch (method) {
        case Method::post:
            return post;
        case Method::get:
            return get;
        case Method::put:
            return put;
        case Method::del:
            return del;
        default:
            ESP_LOGE(tag, "unsopported method");
            throw UnsupportedMethod();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool PathNode::hasHandler(Method method) const {
    return handlerFromMethod(method).operator bool();
}

// ---------------------------------------------------------------------------------------------------------------------

void PathNode::setHandler(Method method, const Handler<std::string> &handler) {
    handlerFromMethod(method) = handler;
}

// ---------------------------------------------------------------------------------------------------------------------

}
