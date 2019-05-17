#pragma once

#include <mfl/httpd/Handler.hpp>
#include <mfl/httpd/Method.hpp>

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

    Handler<std::string>& handlerFromMethod(Method method);
    const Handler<std::string>& handlerFromMethod(Method method) const;

    bool hasHandler(Method method) const;
    void setHandler(Method method, const Handler<std::string>& handler);

    Handler<std::string> post;
    Handler<std::string> get;
    Handler<std::string> put;
    Handler<std::string> del;
    std::vector<std::string> args;
    std::map<std::string, PathNode> children;
};

// ---------------------------------------------------------------------------------------------------------------------

}
