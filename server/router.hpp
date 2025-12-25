
// this is an example of how one can build there own router 
#include <map>
#include <memory>
#include <string>

#include "interface.hpp"

class router{
   std::map<std::string,std::unique_ptr<baseInterface>> handler; 

   router(){};
   public:
   static router& getRouter(){
      static router rtr;
      return rtr;
   }
     bool set(std::string route, std::unique_ptr<baseInterface> bi){
       handler[std::move(route)] = std::move(bi);
       return true;
   }
   baseInterface* getIntr(const std::string& target){
     if(handler.find(target) != handler.end()) {
         return handler[target].get();
     }
     return nullptr;
   }
   router(router&router)=delete;
   router& operator=(const router&router) = delete;
};