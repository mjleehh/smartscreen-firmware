#pragma once

#include <mfl/httpd/Context.hpp>
#include <mfl/helpers/macros.hpp>

#include <functional>

// ---------------------------------------------------------------------------------------------------------------------

#define MFL_HTTPD_HANDLER_1(code) [](mfl::httpd::Context<std::string>& ctx) code
#define MFL_HTTPD_HANDLER_2(capture, code) [capture](mfl::httpd::Context<std::string>& ctx) code
#define MFL_HTTPD_HANDLER(...) \
    MFL_HELPERS_GET_ARG_3(__VA_ARGS__, MFL_HTTPD_HANDLER_2, MFL_HTTPD_HANDLER_1)(__VA_ARGS__)

// ---------------------------------------------------------------------------------------------------------------------

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

template<typename InT, typename OutT = InT>
using Handler = std::function<void(Context<InT, OutT>& ctx)>;

// ---------------------------------------------------------------------------------------------------------------------

}
