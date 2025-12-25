#pragma once
#include <atomic>
#include <semaphore>
#include <memory>
#include "tcpListner.h"

#include "router.hpp"


class server{
   private:
   std::atomic<bool> open=false;
    std::counting_semaphore<5> sm{5};
   router &rtr;// every thread has a reference to the router object ;
   server():rtr(router::getRouter()){};
   public :
   void readerAndWriterForOne(std::unique_ptr<LineChannel> channel,int client_sck_id);
   int listener(uint16_t port);
   // singleton
   static server& serveMe(){
       static server srv;
       return srv;
   }
   server(server&server)=delete;
   server& operator = (server&server)=delete;// get rid of copy assingnments only one server in one progam

};