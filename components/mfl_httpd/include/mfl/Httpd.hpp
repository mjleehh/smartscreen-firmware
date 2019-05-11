#pragma once

#include <lwip/ip_addr.h>
#include <esp_event.h>
#include <esp_err.h>

#include <map>
#include <functional>
#include <vector>

namespace mfl {

// ---------------------------------------------------------------------------------------------------------------------

struct Httpd {
    using Params = std::map<std::string, std::string>;
    using Handler = std::function<esp_err_t(const std::string& url, Params params)>;
    using Handlers = std::vector<std::pair<std::string, Handler>>;

    explicit Httpd(uint16_t port = 80);
    ~Httpd();

    esp_err_t start();

private:
    uint16_t port_;
    Handlers postHandlers_;
    Handlers getHandlers_;
    Handlers putHandlers_;
    Handlers deleteHandlers_;
};

// ---------------------------------------------------------------------------------------------------------------------

} // namespace mfl
