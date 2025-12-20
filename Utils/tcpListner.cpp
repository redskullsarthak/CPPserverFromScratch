#include "tcpListner.h"

int readTCP(LineChannel & channel,int sck_fd) { //sck_fd==socket file descriptor 
    char buffer[8];
    std::string s;
    while (true) {
        auto sz=recv(sck_fd,buffer,sizeof(buffer),0); // instead of file read the data into a buffer from a socket 
        if (sz > 0) {
           for (int i = 0; i < sz; i++) {
              if (buffer[i] != '\n') {
                  s.push_back(buffer[i]);
              } else {
                channel.push(s);
                s.clear();
              }
           }
        }
        else break;// some error or read everything and connection closed 
    }
    if (!s.empty()) channel.push(s);
    channel.close();
    return 0;
}

int runTCPLineServer(uint16_t port) {
    LineChannel channel;
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
            std::cout<<line<<std::endl;
        }
        readerThread.join();
        close(client_fd);
    }

    close(server_fd);
    return 0;
}