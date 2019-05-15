#pragma once

#include <mfl/httpd/Context.hpp>
#include <mfl/helpers/macros.hpp>

#include <functional>

// ---------------------------------------------------------------------------------------------------------------------

#define MFL_HTTPD_HANDLER_1(code) [](mfl::httpd::Context& ctx) code
#define MFL_HTTPD_HANDLER_2(capture, code) [capture](mfl::httpd::Context& ctx) code
#define MFL_HTTPD_HANDLER(...) \
    MFL_HELPERS_GET_ARG_3(__VA_ARGS__, MFL_HTTPD_HANDLER_2, MFL_HTTPD_HANDLER_1)(__VA_ARGS__)

// ---------------------------------------------------------------------------------------------------------------------

namespace mfl {

namespace httpd {

// ---------------------------------------------------------------------------------------------------------------------

using Handler = std::function<void(Context& ctx)>;

// ---------------------------------------------------------------------------------------------------------------------

} // namespace httpd

} // namespace mfl
