#pragma once

#include <stdexcept>

namespace mfl::http {

// ---------------------------------------------------------------------------------------------------------------------

struct UnsupportedMethod : std::invalid_argument {
    explicit UnsupportedMethod() : invalid_argument("the method is not supported"){};
};

// ---------------------------------------------------------------------------------------------------------------------

enum class Method {
    post,
    get,
    put,
    del,
};

// ---------------------------------------------------------------------------------------------------------------------

}
