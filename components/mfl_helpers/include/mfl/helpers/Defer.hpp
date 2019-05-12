#pragma once

#include <functional>

#define MFL_HELPERS_DEFER(code) ::mfl::helpers::Defer defer ## __LINE__ ([&]() code)

namespace mfl {

namespace helpers {

// ---------------------------------------------------------------------------------------------------------------------

struct Defer {
    explicit Defer(std::function<void()> f) : f_(std::move(f)) {}

    ~Defer() {
        f_();
    }

private:
    std::function<void()> f_;
};

// ---------------------------------------------------------------------------------------------------------------------

} // namespace helpers

} // namespace mfl

