#include "MainView.hpp"

#include <sstream>

namespace smartscreen {

namespace {

// ---------------------------------------------------------------------------------------------------------------------

const char tag[] = "main view";
ip4_addr nullIp = {0};

// ---------------------------------------------------------------------------------------------------------------------

} // namespace anonymous

// ---------------------------------------------------------------------------------------------------------------------

MainView::MainView(mfl::Display &display) : ip_(nullIp), display_(display) {

}

// ---------------------------------------------------------------------------------------------------------------------

esp_err_t MainView::draw() {
    auto h = display_.handle();
    u8g2_ClearBuffer(h);
    u8g2_SetFont(h, u8g2_font_profont12_mf);

    std::string ipString = "-";
    if (ip_.addr != 0) {
        auto ip = (char *) &ip_;
        std::stringstream ss;
        ss << (int) ip[0] << "." << (int) ip[1] << "." << (int) ip[2] << "." << (int) ip[3];
        ipString = ss.str();
    }
    u8g2_DrawStr(h, 2, 17, message_.c_str());
    u8g2_DrawStr(h, 2, 2 * 17, ipString.c_str());
    u8g2_DrawStr(h, 128 - 17, 2 * 17, wifiStatus_.c_str());
    u8g2_SendBuffer(h);
    return ESP_OK;
}

// ---------------------------------------------------------------------------------------------------------------------

esp_err_t MainView::setIp(const ip4_addr &ip) {
    ip_ = ip;
    return draw();
}

// ---------------------------------------------------------------------------------------------------------------------

esp_err_t MainView::setMessage(const std::string &message) {
    message_ = message;
    return draw();
}

// ---------------------------------------------------------------------------------------------------------------------

esp_err_t MainView::setWifiStatus(const std::string& status) {
    wifiStatus_ = status;
    return draw();
}

// ---------------------------------------------------------------------------------------------------------------------

} // smartscreen

