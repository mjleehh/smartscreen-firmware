set(COMPONENT_SRCDIRS "src")
set(COMPONENT_PRIV_INCLUDEDIRS "src")
set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_REQUIRES mfl_http)
set(COMPONENT_PRIV_REQUIRES esp_http_client)

register_component()
