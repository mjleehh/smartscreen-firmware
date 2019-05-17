#pragma once

#include <mfl/httpd/HttpLayer.hpp>
#include <mfl/httpd/Context.hpp>
#include <mfl/httpd/PathNode.hpp>
#include <mfl/httpd/Method.hpp>
#include <mfl/httpd/response_types/response-types.hpp>

namespace mfl::httpd {

// ---------------------------------------------------------------------------------------------------------------------

struct Router {
    Router();

    template<typename T>
    void post(const std::string& uriTemplate, const Handler<T>& handler) {
        addHandler(Method::post, uriTemplate, handler);
    }

    template<typename T>
    void get(const std::string& uriTemplate, const Handler<T>& handler) {
        addHandler(Method::get, uriTemplate, handler);
    }

    template<typename T>
    void put(const std::string& uriTemplate, const Handler<T>& handler) {
        addHandler(Method::put, uriTemplate, handler);
    }

    template<typename T>
    void del(const std::string& uriTemplate, const Handler<T>& handler) {
        addHandler(Method::del, uriTemplate, handler);
    }

    void handle(Context<std::string>& context) const;
private:
    template<typename T>
    void addHandler(Method method, const std::string& uriTemplate, const Handler<T>& handler) {
        addPlainHandler(method, uriTemplate, [handler](Context<std::string>& c){
            T body = response_types::deserialize<T>(c.body);
            Context<T> newC;
            newC.method = c.method;
            newC.uri = c.uri;
            newC.body = body;
            handler(newC);
            c.res.body = response_types::serialize<T>(newC.res.body);
        });
    }

    void addPlainHandler(Method method, const std::string& uriTemplate, const Handler<std::string>& handler);

    PathNode root_;
};

// ---------------------------------------------------------------------------------------------------------------------

}
