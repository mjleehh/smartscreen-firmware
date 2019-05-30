#!/usr/bin/env sh

openocd -f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp-wroom-32.cfg
