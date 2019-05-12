#pragma once

#include <driver/gpio.h>
#include <driver/i2c.h>
#include <u8g2.h>


namespace mfl {

// ---------------------------------------------------------------------------------------------------------------------

/**
 * I2C displays
 */
struct Display {
    enum class ControllerType {
        ssd1306
    };

    static const gpio_num_t UNDEFINED_PIN = GPIO_NUM_MAX;

    Display(ControllerType controllerType, gpio_num_t clockPin, gpio_num_t dataPin, gpio_num_t reset = UNDEFINED_PIN, gpio_num_t dc = UNDEFINED_PIN);

    /**
     * Get the raw U8G2 display library handle.
     */
    u8g2_t *handle() {
        return (u8g2_t *) (this);
    }

private:
    /**
     * Convert from nested Display::*handle_ member pointer to Display* of that object.
     *
     * WARNING: This is hacky. If the handle argument is not of type Display::*handle there be dragons!
     */
    static Display* displayFromHandle(u8x8_t* handle);

    static uint8_t byteCb(u8x8_t *handle, uint8_t msg, uint8_t arg_int, void *arg_ptr);
    static uint8_t delayCb(u8x8_t *handle, uint8_t msg, uint8_t arg_int, void *arg_ptr);

    u8g2_t handle_;
    gpio_num_t clockPin_;
    gpio_num_t dataPin_;
    gpio_num_t reset_;
    gpio_num_t dc_;
    i2c_cmd_handle_t i2cHandle_;
};

// ---------------------------------------------------------------------------------------------------------------------

} // namespace mfl
