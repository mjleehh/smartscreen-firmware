#pragma once

#include <mfl/httpd/Handler.hpp>
#include <mfl/http/Method.hpp>

#include <map>
#include <vector>

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

struct PathNode {
    PathNode(std::vector<std::string>&& args);

    Handler<std::string>& handlerFromMethod(http::Method method);
    const Handler<std::string>& handlerFromMethod(http::Method method) const;

    bool hasHandler(http::Method method) const;
    void setHandler(http::Method method, const Handler<std::string>& handler);

    Handler<std::string> post;
    Handler<std::string> get;
    Handler<std::string> put;
    Handler<std::string> del;
    std::vector<std::string> args;
    std::map<std::string, PathNode> children;
};

// ---------------------------------------------------------------------------------------------------------------------

}
