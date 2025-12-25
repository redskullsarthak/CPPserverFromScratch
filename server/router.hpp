
// this is an example of how one can build there own router 
#include<map>
#include<memory>
#include <string>
#include<interface.hpp>

class router{
   std::map<std::string,std::unique_ptr<baseInterface>> handler; 

   router(){};
   public:
   static router& getRouter(){
      static router rtr;
      return rtr;
   }
   bool set(std::string route,baseInterface & bi){
       std::unique_ptr<baseInterface> biptr=std::make_unique<baseInterface>(bi);
       handler[route]=std::move(biptr);
   }
   std::unique_ptr<baseInterface> getIntr(std::string target){
     return std::move(handler[target]);
   }
   router(router&router)=delete;
   router& operator=(const router&router) = delete;
};