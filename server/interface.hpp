#pragma once

#include <string>
#include "tcpListner.h"

class baseInterface{

    public : 
  virtual ~baseInterface() = default;
      virtual bool get(HttpReq &hr)=0;
      virtual bool post(HttpReq &hr)=0;
      virtual bool patch(HttpReq &hr)=0;
      virtual bool del(HttpReq &hr)=0;
}; // factory which can be implemented from ;

