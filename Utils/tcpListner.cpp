#include "tcpListner.h"
#include<map>
#include <fstream>

//http req
// req-linre \r\n
// field lines\r\n
// \r\n
// body
struct HttpReq{
  std::string requestLine{};
  std::map<std::string,std::string> headers{};// field-lines;
  std::string body{}; 
};

void parser(std::string &line, HttpReq& hr, int &state) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    if (state == 1) {
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
        std::string key{};
        std::string value{};
        for(int i=0;i<colonPos;i++){
            if(line[i]==' ') continue;
            else key.push_back(line[i]);
        }
        for(int i=colonPos+2;i<line.length();i++){
           if(line[i]==' ') break;
           value.push_back(line[i]);
        }
         hr.headers[key]=value;
        }
    } 
    else {
        hr.body += line ;
    }
}

int readTCP(LineChannel & channel,int sck_fd) { //sck_fd==client socket file descriptor 
    char buffer[4096];// faster than 8 bytes as recv is a system call , mode switch overhead avoided 512 times 
    std::string s;//(state machine)
    HttpReq httprequest;
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
    for(auto &kp:httprequest.headers){
        std::cout<<kp.first<<" "<<kp.second<<std::endl;
    }
    channel.close();
    return 0;
}

int runTCPLineServer(uint16_t port) {
    LineChannel channel;
    std::ofstream f("output.txt"); 
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return 1;
    }

    listen(server_fd, 3);
    std::cout << "Server listening on port " << port << "..." << std::endl;
    while(true){     
        int addrlen=sizeof(address);
        int client_fd=accept(server_fd,(struct sockaddr*)&address,(socklen_t*)&addrlen);
        if(!channel.clearChannel()) return 1;
        std::thread readerThread(readTCP,std::ref(channel),client_fd); // this thread will be busy trying to listen to the network
        // meanwhile the main thread can process what i want , that is output the lines to stdout
        std::string line;
        while(channel.pop(line)){
            f << line << std::endl;
        }
        readerThread.join();
        close(client_fd);
    }
    
    close(server_fd);
    return 0;
}