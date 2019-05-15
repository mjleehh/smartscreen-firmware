#include <mfl/httpd/PathNode.hpp>

namespace mfl {

namespace httpd {

// ---------------------------------------------------------------------------------------------------------------------

PathNode::PathNode(std::vector<std::string> &&args) : args(args) {

}

// ---------------------------------------------------------------------------------------------------------------------

Handler &PathNode::handlerFromMethod(httpd_method_t method) {
    switch (method) {
        case HTTP_POST:
            return post;
        case HTTP_GET:
            return get;
        case HTTP_PUT:
            return put;
        case HTTP_DELETE:
            return get;
        default:
            throw UnsupportedMethod();

    }
}

// ---------------------------------------------------------------------------------------------------------------------

const Handler &PathNode::handlerFromMethod(httpd_method_t method) const {
    switch (method) {
        case HTTP_POST:
            return post;
        case HTTP_GET:
            return get;
        case HTTP_PUT:
            return put;
        case HTTP_DELETE:
            return get;
        default:
            throw UnsupportedMethod();

    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool PathNode::hasHandler(httpd_method_t method) const {
    return handlerFromMethod(method).operator bool();
}

// ---------------------------------------------------------------------------------------------------------------------

void PathNode::setHandler(httpd_method_t method, const Handler &handler) {
    handlerFromMethod(method) = handler;
}

// ---------------------------------------------------------------------------------------------------------------------

} // namespace httpd

} // namespace mfl
