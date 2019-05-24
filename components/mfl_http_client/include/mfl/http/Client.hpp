#pragma once

#include <string>
#include <mfl/http/Method.hpp>
#include <mfl/http/response_types/response-types.hpp>

namespace mfl::http {

// ---------------------------------------------------------------------------------------------------------------------

struct Client {
    Client(Method method, const std::string& uri);

    template<typename T>
    Client& body(const T& body) {
        return setRawBody(
                response_types::serialize(body),
                response_types::getMimeType<T>()
        );
    }

    template<typename T>
    T request() {
        return response_types::deserialize<T>(rawRequest());
    }

    ~Client();

private:
    Client& setRawBody(std::string&& body, const std::string& mimeType);
    std::string rawRequest();

    std::string body_;
    void* client_;
};

// ---------------------------------------------------------------------------------------------------------------------

}
