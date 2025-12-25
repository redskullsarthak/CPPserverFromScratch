// cmd/main.cpp
#include "server.hpp"
#include "coffeehandler.hpp"

int main() {
    router &rtr = router::getRouter();
    auto myHandler = std::make_unique<CoffeeHandler>();
    rtr.set("/coffee", std::move(myHandler));
    server& myServer = server::serveMe();
    myServer.listener(42069);
    return 0;
}