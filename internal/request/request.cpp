#include "request.hpp"

#include <stdexcept>

namespace request {

static RequestLine parse_request_line(const std::string& line) {
    // Expected: METHOD SP REQUEST_TARGET SP HTTP/VERSION
    // Example:  GET / HTTP/1.1
    auto first_space = line.find(' ');
    if (first_space == std::string::npos) {
        throw std::runtime_error("invalid request line: missing spaces");
    }

    auto second_space = line.find(' ', first_space + 1);
    if (second_space == std::string::npos) {
        throw std::runtime_error("invalid request line: missing second space");
    }

    RequestLine rl;
    rl.method = line.substr(0, first_space);
    rl.request_target = line.substr(first_space + 1, second_space - (first_space + 1));

    const std::string http_prefix = "HTTP/";
    if (line.compare(second_space + 1, http_prefix.size(), http_prefix) != 0) {
        throw std::runtime_error("invalid request line: missing HTTP/ prefix");
    }

    rl.http_version = line.substr(second_space + 1 + http_prefix.size());
    return rl;
}

HttpRequest parse_request(const std::string& raw_request) {
    // Minimal parser for now: only request line.
    // The request line ends at the first CRLF (\r\n).
    auto crlf = raw_request.find("\r\n");
    std::string first_line = (crlf == std::string::npos) ? raw_request : raw_request.substr(0, crlf);

    HttpRequest req;
    req.request_line = parse_request_line(first_line);
    return req;
}

} // namespace request
