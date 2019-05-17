#pragma once

#include <mfl/httpd/Response.hpp>
#include <mfl/httpd/Method.hpp>

#include <map>

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

using Params = std::map<std::string, std::string>;

// ---------------------------------------------------------------------------------------------------------------------

template<typename InT, typename OutT = InT>
struct Context {
    Method method;
    std::string uri;
    Params params;

    InT body;
    Response<OutT> res;
};

// ---------------------------------------------------------------------------------------------------------------------

}
