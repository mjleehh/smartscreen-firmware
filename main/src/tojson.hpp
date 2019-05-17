#pragma once

#include <mfl/httpd/Response.hpp>
#include <nlohmann/json.hpp>

namespace mfl::httpd::response_types {

// ---------------------------------------------------------------------------------------------------------------------

template<>
std::string typeString<nlohmann::json>() {
    return "application/json";
}

// ---------------------------------------------------------------------------------------------------------------------

template<>
std::string serialize(const nlohmann::json& resBody) {
    return resBody.dump();
}

// ---------------------------------------------------------------------------------------------------------------------

template<>
nlohmann::json deserialize(const std::string& reqBody) {
    return nlohmann::json::parse(reqBody);
}
// ---------------------------------------------------------------------------------------------------------------------

}
