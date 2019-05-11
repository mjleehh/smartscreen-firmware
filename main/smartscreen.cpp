#include <mfl/Wifi.hpp>
#include <esp_log.h>
#include <nvs_flash.h>
#include <mfl/Httpd.hpp>


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

using Foo = std::array<uint8_t, 2>;

void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();

    mfl::Wifi wifi(HOSTNAME, SSID, WIFI_PASSWORD);
    mfl::Httpd app(80);

    wifi.start(
            [&wifi, &app](const ip4_addr &addr) {
                ESP_LOGI(tag, "wifi started successfully");
                wifi.addService("screen", mfl::Wifi::Protocol::tcp, 80, "foo");
                ESP_ERROR_CHECK(app.start());
            },
            [] {
                ESP_LOGI(tag, "failed to start wifi");
            }
    );

    for (;;) {
        vTaskDelay(100);
    }

    esp_restart();
}

}
