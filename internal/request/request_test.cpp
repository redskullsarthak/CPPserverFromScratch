#include "request.hpp"
#include <cassert>
#include <iostream>

void test_request_line_parse() {
    // Simple declarative test matching the assignment's spirit
    std::string raw = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    auto req = request::parse_request(raw);

    // Using standard C++ assertions (The "C++ Testify")
    assert(req.request_line.method == "GET");
    assert(req.request_line.request_target == "/");
    assert(req.request_line.http_version == "1.1");

    std::cout << "TestRequestLineParse: PASSED" << std::endl;
}

int main() {
    test_request_line_parse();
    return 0;
}