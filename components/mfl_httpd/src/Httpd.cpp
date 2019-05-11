#include <mfl/Httpd.hpp>
#include <http_server.h>

namespace mfl {

namespace {

// ---------------------------------------------------------------------------------------------------------------------

const char tag[] = "mfl_httpd";

httpd_handle_t activeServer = nullptr;

bool matchAny(const char *reference_uri, const char *uri_to_match, size_t match_upto) {
    return true;
}

esp_err_t postHandler(httpd_req_t *req) {
    auto res =  "URI POST Response";
    httpd_resp_send(req, res, strlen(res));
    return ESP_OK;
}


httpd_uri_t postRequest = {
        .uri      = "*",
        .method   = HTTP_POST,
        .handler  = postHandler,
        .user_ctx = nullptr
};

esp_err_t getHandler(httpd_req_t *req) {
    auto res =  "URI GET Response";
    httpd_resp_send(req, res, strlen(res));
    return ESP_OK;
}

httpd_uri_t getRequest = {
        .uri      = "*",
        .method   = HTTP_GET,
        .handler  = getHandler,
        .user_ctx = nullptr
};

esp_err_t putHandler(httpd_req_t *req) {
    auto res =  "URI PUT Response";
    httpd_resp_send(req, res, strlen(res));
    return ESP_OK;
}

httpd_uri_t putRequest = {
        .uri      = "*",
        .method   = HTTP_PUT,
        .handler  = putHandler,
        .user_ctx = nullptr
};


esp_err_t deleteHandler(httpd_req_t *req) {
    auto res =  "URI DELETE Response";
    httpd_resp_send(req, res, strlen(res));
    return ESP_OK;
}

httpd_uri_t deleteRequest = {
        .uri      = "*",
        .method   = HTTP_DELETE,
        .handler  = putHandler,
        .user_ctx = nullptr
};

//std::vector<std::string> splitUrl(const std::string& url) {
//    std::vector<std::string> segments;
//    auto startPos = 0;
//    auto pos = 0;
//    while (++pos != url.end()) {
//        if (*pos == '/') {
//            segments.push_back(url.substr(startPos, pos));
//        }
//    }
//}

// ---------------------------------------------------------------------------------------------------------------------

} // namespace anonymous

// ---------------------------------------------------------------------------------------------------------------------


///* Our URI handler function to be called during GET /uri request */
//esp_err_t get_handler(httpd_req_t *req)
//{
//    /* Send a simple response */
//    const char[] resp = "URI GET Response";
//    httpd_resp_send(req, resp, strlen(resp));
//    return ESP_OK;
//}
//
///* Our URI handler function to be called during POST /uri request */
//esp_err_t post_handler(httpd_req_t *req)
//{
//    /* Destination buffer for content of HTTP POST request.
//     * httpd_req_recv() accepts char* only, but content could
//     * as well be any binary data (needs type casting).
//     * In case of string data, null termination will be absent, and
//     * content length would give length of string */
//    char[100] content;
//
//    /* Truncate if content length larger than the buffer */
//    size_t recv_size = MIN(req->content_len, sizeof(content));
//
//    int ret = httpd_req_recv(req, content, recv_size);
//    if (ret <= 0) {  /* 0 return value indicates connection closed */
//        /* Check if timeout occurred */
//        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
//            /* In case of timeout one can choose to retry calling
//             * httpd_req_recv(), but to keep it simple, here we
//             * respond with an HTTP 408 (Request Timeout) error */
//            httpd_resp_send_408(req);
//        }
//        /* In case of error, returning ESP_FAIL will
//         * ensure that the underlying socket is closed */
//        return ESP_FAIL;
//    }
//
//    /* Send a simple response */
//    const char[] resp = "URI POST Response";
//    httpd_resp_send(req, resp, strlen(resp));
//    return ESP_OK;
//}
//
//httpd_uri_t uri_get = {
//        .uri      = "/uri",
//        .method   = HTTP_GET,
//        .handler  = get_handler,
//        .user_ctx = NULL
//};
//
//httpd_uri_t uri_post = {
//        .uri      = "/uri",
//        .method   = HTTP_POST,
//        .handler  = post_handler,
//        .user_ctx = NULL
//};
//


Httpd::Httpd(uint16_t port)
    : port_(port)
{

}

// ---------------------------------------------------------------------------------------------------------------------

Httpd::~Httpd() {
    if (activeServer != nullptr) {
        httpd_stop(activeServer);
    }
}






// ---------------------------------------------------------------------------------------------------------------------

esp_err_t Httpd::start() {
    if (activeServer != nullptr) {
        return ESP_FAIL;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = port_;
    config.uri_match_fn = matchAny;
    auto ret =  httpd_start(&activeServer, &config);
    httpd_register_uri_handler(activeServer, &postRequest);
    httpd_register_uri_handler(activeServer, &getRequest);
    httpd_register_uri_handler(activeServer, &putRequest);
    httpd_register_uri_handler(activeServer, &deleteRequest);
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

} // namespace mfl
