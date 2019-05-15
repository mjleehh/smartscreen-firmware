#include <mfl/Wifi.hpp>
#include <esp_log.h>
#include <nvs_flash.h>
#include <mfl/Httpd.hpp>
#include <mfl/Display.hpp>
#include "MainView.hpp"
#include <nlohmann/json.hpp>

#include <iostream>


const std::string SSID = "HOTEL DESCANSERIA";
const std::string WIFI_PASSWORD = "2222462192";
const mfl::Wifi::Bssid BSSID = {0x16, 0x18, 0xD6, 0x4F, 0x26, 0xA8};

#define DISPLAY_CLOCK_PIN GPIO_NUM_15
#define DISPLAY_DATA_PIN GPIO_NUM_4
#define DISPLAY_RESET_PIN GPIO_NUM_16

//const std::string SSID = "Bubblebet";
//const std::string WIFI_PASSWORD = "uqgy4241";
// const mfl::Wifi::Bssid BSSID;

const std::string HOSTNAME = "smartscreen";

const char* tag = "main";

extern "C" {

void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();

    mfl::Wifi wifi(HOSTNAME, SSID, WIFI_PASSWORD);
    mfl::Httpd app(80);
    mfl::Display display(mfl::Display::ControllerType::ssd1306, DISPLAY_CLOCK_PIN, DISPLAY_DATA_PIN, DISPLAY_RESET_PIN);
    smartscreen::MainView mainView(display);

    mainView.setMessage("hello bob");
    mainView.setWifiStatus("-");

    wifi.start(
            [&wifi, &app, &mainView](const ip4_addr &addr) {
                ESP_LOGI(tag, "wifi started successfully");
                wifi.addService("screen", mfl::Wifi::Protocol::tcp, 80, "foo");
                mainView.setIp(addr);
                mainView.setWifiStatus("c");

                ESP_ERROR_CHECK(app.start());
                app.get("/", MFL_HTTPD_HANDLER({
                    std::cout << "hello!!!!" << std::endl;
                }));

                app.put("/foo/:arg1/:arg2/foo/:arg3", MFL_HTTPD_HANDLER({
                    std::cout << "hello2!!!!" << std::endl;
                    std::cout << ctx.body << std::endl;
                    for (auto arg : ctx.params) {
                        std::cout << arg.first << " " << arg.second << std::endl;
                    }
                    nlohmann::json body;
                    body["msg"] = "you called to foobar";
                    ctx.res.body = body.dump();
                }));
            },
            [&mainView] {
                ESP_LOGI(tag, "failed to start wifi");
                mainView.setWifiStatus("f");
            }
    );

    for (;;) {
        vTaskDelay(100);
    }

    esp_restart();
}

}
