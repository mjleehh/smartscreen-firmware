#include <mfl/httpd/Router.hpp>
#include <mfl/httpd/Errors.hpp>
#include <mfl/helpers/Defer.hpp>

#include "helpers.hpp"

#include <iostream>
#include <esp_log.h>

namespace mfl::httpd {

namespace {

// ---------------------------------------------------------------------------------------------------------------------

const char tag[] = "mfl_httpd";

// ---------------------------------------------------------------------------------------------------------------------

bool isArg(const std::string& segment) {
    return segment[0] == ':';
}

// ---------------------------------------------------------------------------------------------------------------------

std::string argName(const std::string& segment) {
    return segment.substr(1);
}

// ---------------------------------------------------------------------------------------------------------------------

} // namespace anonymous

// =====================================================================================================================

Router::Router()
        : root_(std::vector<std::string>())
{

}

// ---------------------------------------------------------------------------------------------------------------------

void Router::addPlainHandler(http::Method method, const std::string& uriTemplate, const Handler<std::string>& handler) {
    auto uri = splitUrl(uriTemplate);

    auto *node = &root_;
    auto uriPos = uri.begin();

    while (uriPos != uri.end()) {
        auto &segment = *uriPos;
        if (segment[0] == ':') {
            throw EndpointError("URI arg with no node %s");
        }

        std::vector<std::string> args;
        ++uriPos;
        while (uriPos != uri.end() && isArg(*uriPos)) {
            args.push_back(argName(*uriPos));
            ++uriPos;
        }

        auto &children = node->children;
        auto child = children.find(segment);
        if (child != children.end()) {
            if (args.size() != child->second.args.size()) {
                throw EndpointError("differnt argument number defined for existing node");
            }
            node = &child->second;
        } else {
            node = &children.emplace(segment, std::move(args)).first->second;
        }
    }

    if (node->hasHandler(method)) {
        throw EndpointError("handler already defined for node %s");
    }
    node->setHandler(method, handler);
    ESP_LOGI(tag, "added URI handler for %s", uriTemplate.c_str());
}

// ---------------------------------------------------------------------------------------------------------------------

void Router::handle(Context<std::string>& context) const {
    auto uri = splitUrl(context.uri);
    const auto* node = &root_;

    std::map<std::string, std::string> argValues;
    const PathNode* handlerNode = nullptr;

    MFL_HELPERS_DEFER({
        if (handlerNode && handlerNode->hasHandler(context.method)) {
            context.params = std::move(argValues);
            handlerNode->handlerFromMethod(context.method)(context);
        } else {
            context.res.status = Status::notFound;
        }
    });

    // edge case root
    if (uri.empty()) {
        handlerNode = node;
    }

    try {
        auto uriPos = uri.begin();

        // traverse the tree until, the node described by the URI is encountered
        while (uriPos != uri.end()) {
            node = &node->children.at(*uriPos);
            const auto& args = node->args;
            auto argPos = args.begin();
            ++uriPos;
            while (argPos != args.end()) {
                if (uriPos == uri.end()) {
                    ESP_LOGI(tag, "URI path incomplete");
                    context.res.status = Status::badRequest;
                    return;
                }
                argValues[*argPos] = *uriPos;
                ++argPos;
                ++uriPos;
            }
        }
        handlerNode = node;

    } catch(const std::out_of_range&) {
        throw EndpointError("URI path does not exist");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}
