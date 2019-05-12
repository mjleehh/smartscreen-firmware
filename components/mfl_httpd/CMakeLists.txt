set(COMPONENT_SRCDIRS src)
set(COMPONENT_ADD_INCLUDEDIRS include src)

set(COMPONENT_REQUIRES esp_http_server)
set(COMPONENT_PRIV_REQUIRES mfl_helpers)

register_component()
