#pragma once

#include <map>

namespace mfl::httpd {

enum class Status {
    ok                  = 200,
    badRequest          = 400,
    notFound            = 404,
    internalServerError = 500,
};

// ---------------------------------------------------------------------------------------------------------------------

using Params = std::map<std::string, std::string>;

// ---------------------------------------------------------------------------------------------------------------------

template<typename T>
struct Response {
    Response() : status(Status::ok), returnType("text/plain") {}

    Status status;
    std::string returnType;
    T body;
};

// ---------------------------------------------------------------------------------------------------------------------

}
