#pragma once

namespace mfl::httpd::response_types {

// ---------------------------------------------------------------------------------------------------------------------

template<typename T>
std::string typeString();

// ---------------------------------------------------------------------------------------------------------------------

template<>
inline std::string typeString<std::string>() {
    return "text/plain";
}

// ---------------------------------------------------------------------------------------------------------------------

template<typename T>
std::string serialize(const T& resBody);

// ---------------------------------------------------------------------------------------------------------------------

template<>
inline std::string serialize(const std::string& resBody) {
    return resBody;
}

// ---------------------------------------------------------------------------------------------------------------------

template<typename T>
T deserialize(const std::string& reqBody);

// ---------------------------------------------------------------------------------------------------------------------

template<>
inline std::string deserialize(const std::string& reqBody) {
    return reqBody;
}

// ---------------------------------------------------------------------------------------------------------------------

}