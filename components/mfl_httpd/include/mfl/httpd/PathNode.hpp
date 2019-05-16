#pragma once

#include <mfl/httpd/Handler.hpp>

#include <map>
#include <vector>

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

struct UnsupportedMethod : std::invalid_argument {
    explicit UnsupportedMethod() : invalid_argument("the method is not supported"){};
};

// ---------------------------------------------------------------------------------------------------------------------

struct PathNode {
    PathNode(std::vector<std::string>&& args);

    Handler& handlerFromMethod(httpd_method_t method);
    const Handler& handlerFromMethod(httpd_method_t method) const;

    bool hasHandler(httpd_method_t method) const;
    void setHandler(httpd_method_t method, const Handler& handler);

    Handler post;
    Handler get;
    Handler put;
    Handler del;
    std::vector<std::string> args;
    std::map<std::string, PathNode> children;
};

// ---------------------------------------------------------------------------------------------------------------------

}
