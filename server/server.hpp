#pragma once;
#include<bits/stdc++.h>
#include<semaphore.h>
#include "tcpListner.h"
#include<router.hpp>


class server{
   private:
   std::atomic<bool> open=false;
   sem_t sm;
   router &rtr;// every thread has a reference to the router object ;
   server():rtr(router::getRouter()){sem_init(&sm,0,5);};
   public :
   void readerAndWriterForOne(std::unique_ptr<LineChannel> channel,int client_sck_id);
   int listener(uint16_t port);
   
   // singleton
   std::unique_ptr<server> serveMe(){
       static auto srv=std::make_unique<server>();
       return std::move(srv);
   }
   server(server&server)=delete;
   server& operator = (server&server)=delete;// get rid of copy assingnments only one server in one progam

};