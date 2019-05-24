#pragma once

#include <vector>
#include <string>
#include <esp_err.h>
#include <stdexcept>

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

struct UrlParseError : std::invalid_argument {
    explicit UrlParseError(const std::string& what) : invalid_argument(what) {}
};

// ---------------------------------------------------------------------------------------------------------------------

void printUrl(const std::vector<std::string>& url);

// ---------------------------------------------------------------------------------------------------------------------

std::vector <std::string> splitUrl(const std::string &str);

// ---------------------------------------------------------------------------------------------------------------------

}
