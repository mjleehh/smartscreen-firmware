#include <mfl/Wifi.hpp>
#include <esp_log.h>
#include <nvs_flash.h>
#include <mfl/EspHttpd.hpp>
#include <mfl/httpd/Router.hpp>
#include <mfl/Display.hpp>
#include "MainView.hpp"
#include <tojson.hpp>

#include <iostream>


const std::string SSID = "HOTEL DESCANSERIA";
const std::string WIFI_PASSWORD = "2222462192";
const mfl::Wifi::Bssid BSSID = {0x16, 0x18, 0xD6, 0x4F, 0x26, 0xA8};

#define DISPLAY_CLOCK_PIN GPIO_NUM_15
#define DISPLAY_DATA_PIN GPIO_NUM_4
#define DISPLAY_RESET_PIN GPIO_NUM_16

const std::string HOSTNAME = "smartscreen";

const char* tag = "main";

extern "C" {

void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();

    mfl::Wifi wifi(HOSTNAME, SSID, WIFI_PASSWORD);
    mfl::httpd::Router router;
    mfl::EspHttpd app(router, 80);
    mfl::Display display(mfl::Display::ControllerType::ssd1306, DISPLAY_CLOCK_PIN, DISPLAY_DATA_PIN, DISPLAY_RESET_PIN);
    smartscreen::MainView mainView(display);

    mainView.setMessage("hello bob");
    mainView.setWifiStatus("-");

    wifi.start(
            [&wifi, &app, &router, &mainView](const ip4_addr &addr) {
                ESP_LOGI(tag, "wifi started successfully");
                wifi.addService("screen", mfl::Wifi::Protocol::tcp, 80, "foo");
                mainView.setIp(addr);
                mainView.setWifiStatus("c");

                app.start();

                mfl::httpd::Handler<nlohmann::json> f = [&mainView](mfl::httpd::Context<nlohmann::json>& ctx){
                    try {
                        std::cout << "bla bla" << std::endl;
                        std::cout << ctx.body.dump() << std::endl;
                        auto s = ctx.body["msg"].get<std::string>();
                        std::cout << s << std::endl;
                        mainView.setMessage(s);
                    } catch(...) {
                        std::cout << "bad request" << std::endl;
                    }

                    nlohmann::json res = {};
                    ctx.res.body = res;
                };

                router.put("/foo/bar", f);

                mfl::httpd::Handler<std::string> g = [](mfl::httpd::Context<std::string>&){
                    std::cout << "hello" << std::endl;
                };
                router.get("/blub", g);
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
