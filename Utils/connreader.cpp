// tested this works 

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <utility>
// reads from a file or a connection 8 bytes at a time , only ouptuts 
//after encountering a new line '/n'

class LineChannel {
    std::queue<std::string> q;
    std::condition_variable cv;
    std::mutex mtx;
    bool closed = false;

public:
    void push(std::string s) { 
        std::lock_guard<std::mutex> lock(mtx); // only wait when u have to wait 
        q.push(s);
        cv.notify_one(); // notify one of the sleeping locks on pop
    }
    
    bool pop(std::string &out) {
        std::unique_lock<std::mutex> lock(mtx);
        // Wait until there is a message OR the channel is closed
        cv.wait(lock, [this]{ return !q.empty() || closed; });// avoids busy waiting 

        // If the queue is empty and closed is true, we are done else popping can be done
        if (q.empty() && closed) return false;

        out = q.front();
        q.pop();
        return true; 
    }

    void close() {
        std::lock_guard<std::mutex> lock(mtx);
        closed = true; 
        cv.notify_all(); // Wake up any threads waiting in pop()
    }

    bool clearChannel(){
        std::lock_guard<std::mutex> lock(mtx);
        std::queue<std::string> empty;
        swap(q,empty);
        closed = false;
        cv.notify_all();
        return true;
    }
};

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
int main() {
    LineChannel channel;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(42069);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return 1;
    }

    listen(server_fd, 3);
    std::cout << "Server listening on port 42069..." << std::endl;
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
    }

    return 0;
}