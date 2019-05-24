#pragma once

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

struct ServerError : std::invalid_argument {
    explicit ServerError(const std::string& what) : invalid_argument(what) {};
};

// ---------------------------------------------------------------------------------------------------------------------

struct EndpointError : std::invalid_argument {
    explicit EndpointError(const std::string& what) : invalid_argument(what) {};
};

// ---------------------------------------------------------------------------------------------------------------------

}