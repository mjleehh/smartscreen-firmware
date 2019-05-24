#include <mfl/Wifi.hpp>
#include <esp_log.h>
#include <nvs_flash.h>
#include <mfl/EspHttpd.hpp>
#include <mfl/httpd/Router.hpp>
#include <mfl/Display.hpp>
#include "MainView.hpp"
#include <tojson.hpp>
#include <mfl/http/Client.hpp>

#include <iostream>
#include <esp_http_client.h>

#include <nlohmann/adl_serializer.hpp>

#include <sdkconfig.h>

const std::string HOME_ADDRESS = "http://" CONFIG_SMARTSCREEN_HOME_ADDRESS ":3001/api/" CONFIG_SMARTSCREEN_DEVICE_ID;

const char* tag = "main";

template<int pin>
constexpr gpio_num_t pinFromInt() {
    static_assert(pin >=  GPIO_NUM_0, "pin number can not be nagative");
    static_assert(pin < GPIO_NUM_MAX, "pin number is larger than available pins");
    return (gpio_num_t) pin;
}

using namespace mfl;
using namespace nlohmann;

extern "C" void app_main() {
    ESP_LOGI(tag, "HELLO ESP");

    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();

    Wifi wifi(CONFIG_SMARTSCREEN_HOSTNAME, CONFIG_SMARTSCREEN_WIFI_SSID, CONFIG_SMARTSCREEN_WIFI_PASSWORD);
    httpd::Router router;
    EspHttpd app(router, 80);
    Display display(Display::ControllerType::ssd1306,
            pinFromInt<CONFIG_SMARTSCREEN_DISPLAY_CLOCK_PIN>(),
            pinFromInt<CONFIG_SMARTSCREEN_DISPLAY_DATA_PIN>(),
            pinFromInt<CONFIG_SMARTSCREEN_DISPLAY_RESET_PIN>());
    smartscreen::MainView mainView(display);

    mainView.setMessage("hello bob");
    mainView.setWifiStatus("-");

    wifi.start(
            [&wifi, &app, &router, &mainView](const ip4_addr &addr) {
                ESP_LOGI(tag, "wifi started successfully");
                wifi.addService("screen", mfl::Wifi::Protocol::tcp, 80, "foo");
                mainView.setIp(addr);
                mainView.setWifiStatus("c");

                std::string body = "{ \"msg\" : \"foobar\" }";

                http::Client homeClient(http::Method::put, HOME_ADDRESS);
                homeClient
                    .body(body)
                    .request<std::string>();

                app.start();

                httpd::Handler<nlohmann::json> f = [&mainView](mfl::httpd::Context<nlohmann::json>& ctx){
                    try {
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
                router.put("/message", f);
                },
                [&mainView] {
                ESP_LOGI(tag, "failed to start wifi");
                mainView.setWifiStatus("f");
            }
            );

    for (;;) {
        vTaskDelay(100);
    }
}
