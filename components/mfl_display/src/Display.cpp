#include "../include/mfl/Display.hpp"

#include <esp_log.h>

#define I2C_MASTER_NUM I2C_NUM_1           //  I2C port number for master dev
#define I2C_MASTER_TX_BUF_DISABLE   0      //  I2C master do not need buffer
#define I2C_MASTER_RX_BUF_DISABLE   0      //  I2C master do not need buffer
#define I2C_MASTER_FREQ_HZ          50000  //  I2C master clock frequency
#define ACK_CHECK_EN   0x1                 //  I2C master will check ack from slave
#define ACK_CHECK_DIS  0x0                 //  I2C master will not check ack from slave

namespace mfl {

namespace {

// ---------------------------------------------------------------------------------------------------------------------

const char tag[] = "display";
const unsigned int I2C_TIMEOUT_MS = 1000;

// ---------------------------------------------------------------------------------------------------------------------

} // namespace anonymous

// ---------------------------------------------------------------------------------------------------------------------

Display::Display(ControllerType controllerType, gpio_num_t clockPin, gpio_num_t dataPin, gpio_num_t resetPin, gpio_num_t dcPin)
        : clockPin_(clockPin), dataPin_(dataPin), reset_(resetPin), dc_(dcPin), i2cHandle_(0) {

    u8g2_Setup_ssd1306_i2c_128x64_noname_f(
            &handle_,
            U8G2_R0,
            byteCb,
            delayCb);
    handle_.u8x8.i2c_address = 0x78;

    ESP_LOGI(tag, "u8g2_InitDisplay");
    u8g2_InitDisplay((u8g2_t *) this); // send init sequence to the display, display is in sleep mode after this,

    ESP_LOGI(tag, "u8g2_SetPowerSave");
    u8g2_SetPowerSave((u8g2_t *) this, 0); // wake up display


}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Display::byteCb(u8x8_t *handle, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    auto display = displayFromHandle(handle);

    switch (msg) {
        case U8X8_MSG_BYTE_SET_DC: {
            if (display->dc_ != UNDEFINED_PIN) {
                gpio_set_level(display->dc_, arg_int);
            }
            break;
        }

        case U8X8_MSG_BYTE_INIT: {
            ESP_LOGI(tag, "setting up I2C for display");

            i2c_config_t conf;
            conf.mode = I2C_MODE_MASTER;
            ESP_LOGD(tag, "sda_io_num %d", display->dataPin_);
            conf.sda_io_num = display->dataPin_;
            conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
            ESP_LOGD(tag, "scl_io_num %d", display->clockPin_);
            conf.scl_io_num = display->clockPin_;
            conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
            ESP_LOGD(tag, "clk_speed %d", I2C_MASTER_FREQ_HZ);
            conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
            ESP_LOGD(tag, "i2c_param_config %d", conf.mode);
            ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
            ESP_LOGD(tag, "i2c_driver_install %d", I2C_MASTER_NUM);
            ESP_ERROR_CHECK(
                    i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE,
                                       0));
            break;
        }

        case U8X8_MSG_BYTE_SEND: {
            uint8_t *data_ptr = (uint8_t *) arg_ptr;
            ESP_LOG_BUFFER_HEXDUMP(tag, data_ptr, arg_int, ESP_LOG_VERBOSE);

            while (arg_int > 0) {
                ESP_ERROR_CHECK(i2c_master_write_byte(display->i2cHandle_, *data_ptr, ACK_CHECK_EN));
                data_ptr++;
                arg_int--;
            }
            break;
        }

        case U8X8_MSG_BYTE_START_TRANSFER: {
            uint8_t i2c_address = display->handle_.u8x8.i2c_address;
            display->i2cHandle_ = i2c_cmd_link_create();
            ESP_LOGD(tag, "Start I2C transfer to %02X.", i2c_address >> 1);
            ESP_ERROR_CHECK(i2c_master_start(display->i2cHandle_));
            ESP_ERROR_CHECK(i2c_master_write_byte(display->i2cHandle_, i2c_address | I2C_MASTER_WRITE, ACK_CHECK_EN));
            break;
        }

        case U8X8_MSG_BYTE_END_TRANSFER: {
            ESP_LOGD(tag, "End I2C transfer.");
            ESP_ERROR_CHECK(i2c_master_stop(display->i2cHandle_));
            ESP_ERROR_CHECK(
                    i2c_master_cmd_begin(I2C_MASTER_NUM, display->i2cHandle_, I2C_TIMEOUT_MS / portTICK_RATE_MS));
            i2c_cmd_link_delete(display->i2cHandle_);
            break;
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Display::delayCb(u8x8_t *handle, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    ESP_LOGD(tag, "gpio_and_delay_cb: Received a msg: %d, arg_int: %d, arg_ptr: %p", msg, arg_int, arg_ptr);

    Display *display = (Display *) handle;

    switch (msg) {
        // Initialize the GPIO and DELAY HAL functions.  If the pins for DC and RESET have been
        // specified then we define those pins as GPIO outputs.
        case U8X8_MSG_GPIO_AND_DELAY_INIT: {
            uint64_t bitmask = 0;
            if (display->dc_ != UNDEFINED_PIN) {
                bitmask = bitmask | (1ull << display->dc_);
            }
            if (display->reset_ != UNDEFINED_PIN) {
                bitmask = bitmask | (1ull << display->reset_);
            }

            if (bitmask == 0) {
                break;
            }
            gpio_config_t gpioConfig;
            gpioConfig.pin_bit_mask = bitmask;
            gpioConfig.mode = GPIO_MODE_OUTPUT;
            gpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
            gpioConfig.pull_down_en = GPIO_PULLDOWN_ENABLE;
            gpioConfig.intr_type = GPIO_INTR_DISABLE;
            gpio_config(&gpioConfig);
            break;
        }

            // Set the GPIO reset pin to the value passed in through arg_int.
        case U8X8_MSG_GPIO_RESET:
            if (display->reset_ != UNDEFINED_PIN) {
                gpio_set_level(display->reset_, arg_int);
            }
            break;
            // Set the GPIO client select pin to the value passed in through arg_int.
        case U8X8_MSG_GPIO_I2C_CLOCK:
            if (display->clockPin_ != UNDEFINED_PIN) {
                gpio_set_level(display->clockPin_, arg_int);
                //				printf("%c",(arg_int==1?'C':'c'));
            }
            break;
            // Set the Software I²C pin to the value passed in through arg_int.
        case U8X8_MSG_GPIO_I2C_DATA:
            if (display->dataPin_ != UNDEFINED_PIN) {
                gpio_set_level(display->dataPin_, arg_int);
                //				printf("%c",(arg_int==1?'D':'d'));
            }
            break;

            // Delay for the number of milliseconds passed in through arg_int.
        case U8X8_MSG_DELAY_MILLI:
            vTaskDelay(arg_int / portTICK_PERIOD_MS);
            break;
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

Display* Display::displayFromHandle(u8x8_t* handle) {
    const size_t delta =
            reinterpret_cast<char*>(&static_cast<Display*>(nullptr)->handle_) - static_cast<char*>(nullptr);

    return reinterpret_cast<Display*>(reinterpret_cast<char*>(handle) - delta);
}

// ---------------------------------------------------------------------------------------------------------------------

} // namespace mfl

