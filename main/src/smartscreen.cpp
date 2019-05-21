#include <mfl/Wifi.hpp>
#include <esp_log.h>
#include <nvs_flash.h>
#include <mfl/EspHttpd.hpp>
#include <mfl/httpd/Router.hpp>
#include <mfl/Display.hpp>
#include "MainView.hpp"
#include <tojson.hpp>

#include <iostream>
#include <esp_http_client.h>

#include <sdkconfig.h>

const std::string HOME_ADDRESS = "http://" CONFIG_SMARTSCREEN_HOME_ADDRESS ":3001/api/" CONFIG_SMARTSCREEN_DEVICE_ID;

const char* tag = "main";

template<int pin>
constexpr gpio_num_t pinFromInt() {
    static_assert(pin >=  GPIO_NUM_0, "pin number can not be nagative");
    static_assert(pin < GPIO_NUM_MAX, "pin number is larger than available pins");
    return (gpio_num_t) pin;
}

extern "C" {

esp_err_t phoneHome(esp_http_client_event_t *evt) {
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(tag, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(tag, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(tag, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(tag, "HTTP_EVENT_ON_HEADER");
            printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(tag, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                printf("%.*s", evt->data_len, (char*)evt->data);
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(tag, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(tag, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();

    mfl::Wifi wifi(CONFIG_SMARTSCREEN_HOSTNAME, CONFIG_SMARTSCREEN_WIFI_SSID, CONFIG_SMARTSCREEN_WIFI_PASSWORD);
    mfl::httpd::Router router;
    mfl::EspHttpd app(router, 80);
    mfl::Display display(mfl::Display::ControllerType::ssd1306,
            pinFromInt<CONFIG_SMARTSCREEN_DISPLAY_CLOCK_PIN>(),
            pinFromInt<CONFIG_SMARTSCREEN_DISPLAY_DATA_PIN>(),
            pinFromInt<CONFIG_SMARTSCREEN_DISPLAY_RESET_PIN>());
    smartscreen::MainView mainView(display);

//    mainView.setMessage("hello bob");
//    mainView.setWifiStatus("-");

    wifi.start(
            [&wifi, &app, &router, &mainView](const ip4_addr &addr) {
                ESP_LOGI(tag, "wifi started successfully");
                wifi.addService("screen", mfl::Wifi::Protocol::tcp, 80, "foo");
                mainView.setIp(addr);
                mainView.setWifiStatus("c");

                esp_http_client_config_t config = {
                        //.url = HOME_ADDRESS.c_str(),
                        .host = CONFIG_SMARTSCREEN_HOME_ADDRESS,
                        .port = 3001,
                        .path = "/api/" CONFIG_SMARTSCREEN_DEVICE_ID,
                        //.port = 3001,
                        .method = HTTP_METHOD_PUT,
                        .event_handler = phoneHome,
                        .is_async = false,
                };

                ESP_LOGI(tag, "device home: %s", HOME_ADDRESS.c_str());

                esp_http_client_handle_t client = esp_http_client_init(&config);
                esp_http_client_set_header(client, "Content-Type", "application/json");


                std::string s = "{ \"msg\" : \"foobar\" }";
                esp_http_client_set_post_field(client, s.data(), s.size());

                esp_err_t err = esp_http_client_perform(client);
                if (err == ESP_OK) {
                    ESP_LOGI(tag, "Status = %d, content_length = %d",
                             esp_http_client_get_status_code(client),
                             esp_http_client_get_content_length(client));
                }
                esp_http_client_cleanup(client);

//                app.start();
//
//                mfl::httpd::Handler<nlohmann::json> f = [&mainView](mfl::httpd::Context<nlohmann::json>& ctx){
//                    try {
//                        std::cout << ctx.body.dump() << std::endl;
//                        auto s = ctx.body["msg"].get<std::string>();
//                        std::cout << s << std::endl;
//                        mainView.setMessage(s);
//                    } catch(...) {
//                        std::cout << "bad request" << std::endl;
//                    }
//
//                    nlohmann::json res = {};
//                    ctx.res.body = res;
//                };
//                router.put("/message", f);
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
