#pragma once

#include <mfl/httpd/HttpLayer.hpp>
#include <mfl/httpd/Context.hpp>
#include <mfl/httpd/PathNode.hpp>
#include <mfl/http/Method.hpp>
#include <mfl/http/response_types/response-types.hpp>

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Router middleware for HTTP server.
 *
 * Allows gin like requests.
 */
struct Router {
    Router();

    template<typename T>
    void post(const std::string& uriTemplate, const Handler<T>& handler) {
        addHandler(http::Method::post, uriTemplate, handler);
    }

    template<typename T>
    void get(const std::string& uriTemplate, const Handler<T>& handler) {
        addHandler(http::Method::get, uriTemplate, handler);
    }

    template<typename T>
    void put(const std::string& uriTemplate, const Handler<T>& handler) {
        addHandler(http::Method::put, uriTemplate, handler);
    }

    template<typename T>
    void del(const std::string& uriTemplate, const Handler<T>& handler) {
        addHandler(http::Method::del, uriTemplate, handler);
    }

    void handle(Context<std::string>& context) const;
private:
    template<typename T>
    void addHandler(http::Method method, const std::string& uriTemplate, const Handler<T>& handler) {
        addPlainHandler(method, uriTemplate, [handler](Context<std::string>& c){
            T body = http::response_types::deserialize<T>(c.body);
            Context<T> newC;
            newC.method = c.method;
            newC.uri = c.uri;
            newC.body = body;
            handler(newC);
            c.res.body = http::response_types::serialize<T>(newC.res.body);
        });
    }

    void addPlainHandler(http::Method method, const std::string& uriTemplate, const Handler<std::string>& handler);

    PathNode root_;
};

// ---------------------------------------------------------------------------------------------------------------------

}
