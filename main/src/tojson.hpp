#pragma once

#include <mfl/httpd/Response.hpp>
#include <nlohmann/json.hpp>

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

template<>
std::string serializeResponse(const nlohmann::json& response) {
    return response.dump();
}

// ---------------------------------------------------------------------------------------------------------------------

}
