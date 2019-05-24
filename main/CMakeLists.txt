set(COMPONENT_SRCDIRS src)
set(COMPONENT_ADD_INCLUDEDIRS src)

set(COMPONENT_PRIV_REQUIRES
        nvs_flash
        esp_http_client
        mfl_http_client
        mfl_http
        mfl_wifi
        mfl_esp_httpd
        mfl_display
        modern_json)

register_component()