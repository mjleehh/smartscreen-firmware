#pragma once

#include <lwip/ip4_addr.h>
#include <string>
#include <mfl/Display.hpp>

namespace smartscreen {

// ---------------------------------------------------------------------------------------------------------------------

struct MainView {
    MainView(mfl::Display& display);

    esp_err_t setMessage(const std::string& message);

    esp_err_t setWifiStatus(const std::string& status);

    esp_err_t setIp(const ip4_addr& ip);

private:
    esp_err_t draw();

    ip4_addr ip_;
    std::string message_;
    std::string wifiStatus_;
    mfl::Display &display_;
};

// ---------------------------------------------------------------------------------------------------------------------

} // namespace smartscreen
