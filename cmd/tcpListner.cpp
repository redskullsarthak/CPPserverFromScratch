#include "tcpListner.h"
#include<map>
#include <fstream>
//http req
// req-linre \r\n
// field lines\r\n
// \r\n
// body
static inline void ltrim_inplace(std::string &s) {
    while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) {
        s.erase(s.begin());
    }
}

static inline void rtrim_inplace(std::string &s) {
    while (!s.empty() && (s.back() == ' ' || s.back() == '\t')) {
        s.pop_back();
    }
}

static inline void trim_inplace(std::string &s) {
    ltrim_inplace(s);
    rtrim_inplace(s);
}

static void parseToParts(std::string &method, std::string &target, std::string &version, HttpReq& hr) {
    if (hr.requestLine.empty()) return;
    size_t firstSpace = hr.requestLine.find(' ');
    if (firstSpace == std::string::npos) return;
    method = hr.requestLine.substr(0, firstSpace);
    size_t secondSpace = hr.requestLine.find(' ', firstSpace + 1);
    if (secondSpace == std::string::npos) {
        target = hr.requestLine.substr(firstSpace + 1);
        return;
    }
    target = hr.requestLine.substr(firstSpace + 1, secondSpace - (firstSpace + 1));
    version = hr.requestLine.substr(secondSpace + 1);
}

void parser(std::string &line, HttpReq& hr, int &state) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    if (state == 1) {
        trim_inplace(line);
        hr.requestLine = line;
        state = 2;
    } 
    else if (state == 2) {
        if (line.empty()) {
            state = 3;
            return;
        }
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);

            trim_inplace(key);
            trim_inplace(value);

            // normalize key to lowercase for easier matching
            for (char &ch : key) {
                ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
            }

            if (auto it = hr.headers.find(key); it != hr.headers.end()) {
                if (!value.empty()) it->second += ", " + value;
            } else {
                hr.headers[key] = value;
            }
        }
    } 
    else {
        hr.body += line ;
    }
}

int readTCP(LineChannel & channel,int sck_fd,HttpReq &httprequest) { //sck_fd==client socket file descriptor 
    char buffer[4096];// faster than 8 bytes as recv is a system call , mode switch overhead avoided 512 times 
    std::string s;//(state machine)
    int cnt=1;
    while (true) {
        auto sz=recv(sck_fd,buffer,sizeof(buffer),0); // instead of file read the data into a buffer from a socket ,return 0 when connection is closed 
        if (sz > 0) {
           for (int i = 0; i < sz; i++) {
              if (buffer[i] != '\n') {
                  s.push_back(buffer[i]);
              } else {
                channel.push(s);
                parser(s,httprequest,cnt);
                s.clear();
              }
           }
        }
        else break;// some error or read everything and connection closed 
    }
    if (!s.empty()) {
        channel.push(s);
        parser(s,httprequest,cnt);
        s.clear();
    }
    // std::cout<<"method :"<<method<<std::endl<<"target :"<<target<<std::endl<<"version :"<<version<<std::endl;
    // for(auto &kp:httprequest.headers){
    //     std::cout<<kp.first<<" "<<kp.second<<std::endl;
    // }
    try{
    if(stoi(httprequest.headers["content-length"])==httprequest.body.size()){
        std::cout<<"Match"<<std::endl;
        auto sz=send(sck_fd,"Match",5,0);
    }
    else {
        std::cout<<"Content length does not match"<<std::endl;
        auto sz=send(sck_fd,"MisMatch",8,0);
    }
    }
    catch (...){
       std::cout<<" some error while trying"<<std::endl;
    }
    channel.close();
    return 0;
}