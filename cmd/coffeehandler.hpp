
// this is a testing file 
#include "interface.hpp"
#include <iostream>

class CoffeeHandler : public baseInterface {
public:
    bool get(HttpReq &hr) override {
        std::cout << "Handling GET /coffee" << std::endl;
        // i can use send() here to respond to the client_sck_id
        return true;
    }
    bool post(HttpReq &hr) override {
        std::cout << "Handling POST /coffee with body: " << hr.body << std::endl;
        return true;
    }
    // Implement other pure virtuals (patch, del) as empty for now
    bool patch(HttpReq &hr) override { return true; }
    bool del(HttpReq &hr) override { return true; }
};