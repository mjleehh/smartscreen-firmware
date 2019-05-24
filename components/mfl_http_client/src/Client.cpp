#include <mfl/http/Client.hpp>
#include <mfl/http/Method.hpp>
#include <esp_http_client.h>
#include <esp_log.h>

namespace mfl::http {

namespace {

// ---------------------------------------------------------------------------------------------------------------------

const char* tag = "mfl_http_client";

// ---------------------------------------------------------------------------------------------------------------------

esp_http_client_method_t convertMethod(Method method) {
    switch (method) {
        case Method::post:
            return HTTP_METHOD_POST;
        case Method::get:
            return HTTP_METHOD_GET;
        case Method::put:
            return HTTP_METHOD_PUT;
        case Method::del:
            return HTTP_METHOD_DELETE;
        default:
            throw UnsupportedMethod();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

esp_err_t eventHandler(esp_http_client_event_t *evt) {
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

// ---------------------------------------------------------------------------------------------------------------------

inline esp_http_client_handle_t handle(void* ptr) {
    return reinterpret_cast<esp_http_client_handle_t >(ptr);
}

// ---------------------------------------------------------------------------------------------------------------------

}

// ---------------------------------------------------------------------------------------------------------------------

Client::Client(Method method, const std::string &uri) {
    esp_http_client_config_t config = {
            .url = uri.c_str(),
            .method = convertMethod(method),
            .event_handler = eventHandler,
            .is_async = false,
    };
    client_ = esp_http_client_init(&config);
}

// ---------------------------------------------------------------------------------------------------------------------

Client::~Client() {
    esp_http_client_cleanup(handle(client_));
}

// ---------------------------------------------------------------------------------------------------------------------

Client &Client::setRawBody(std::string &&body, const std::string& mimeType) {
    body_ = body;
    ESP_LOGI(tag, "body:'%s'", body_.c_str());
    esp_http_client_set_header(handle(client_), "Content-Type", mimeType.c_str());
    esp_http_client_set_post_field(handle(client_), body_.data(), body_.size());
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string Client::rawRequest() {
    esp_err_t err = esp_http_client_perform(handle(client_));
    ESP_LOGI(tag, "so far");
    if (err == ESP_OK) {
        ESP_LOGI(tag, "Status = %d, content_length = %d",
                 esp_http_client_get_status_code(handle(client_)),
                 esp_http_client_get_content_length(handle(client_)));
    }
    return "foo";
}

// ---------------------------------------------------------------------------------------------------------------------

}
