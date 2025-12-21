#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>

namespace request {
    struct RequestLine {
        std::string method;
        std::string request_target;
        std::string http_version;
    };

    struct HttpRequest {
        RequestLine request_line;
        // Headers and Body will come in future lessons
    };

    // This is the function we will test
    HttpRequest parse_request(const std::string& raw_request);
}

#endif