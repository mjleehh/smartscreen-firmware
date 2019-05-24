#include <esp_log.h>
#include "helpers.hpp"

#include <iostream>

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

void printUrl(const std::vector<std::string>& url) {
    for (auto& segment : url) {
        std::cout << segment << std::endl;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::string feedSegemnt(std::string::const_iterator& pos, std::string::const_iterator end) {
    if (*pos != '/') {
        throw UrlParseError("segment does not start with an slash");
    }

    ++pos;
    auto startPos = pos;
    while (pos != end) {
        if (*pos == '/') {
            return std::string(startPos, pos);
        }
        ++pos;
    }
    return std::string(startPos, pos);
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::string> splitUrl(const std::string &str) {
    std::vector<std::string> res;
    // edge case emtpy URL
    if (str.empty()) {
        throw UrlParseError("emtpy URL");
    }

    // edge case root URL
    if (str == "/") {
        return res;
    }

    auto pos = str.begin();
    auto end = str.end();

    // truncate trailing slashes for non empty strings
    if (pos != end && *(end - 1) == '/') {
        --end;
    }

    while (pos != end) {
        auto segemnt = feedSegemnt(pos, end);
        if (segemnt.empty()) {
            throw UrlParseError("emtpy segment");
        }
        res.push_back(segemnt);
    }
    return res;
}

// ---------------------------------------------------------------------------------------------------------------------

}
