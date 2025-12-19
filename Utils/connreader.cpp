// tested this works 

#include<bits/stdc++.h>
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

        // If the queue is empty and closed is true, we are done
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
};

int readTCP(LineChannel & channel) {
    std::ifstream f("messages.txt");
    if (!f.is_open()) return -1;

    char buffer[8];
    std::string s;
    while (true) {
        f.read(buffer, 8);
        std::streamsize sz = f.gcount();
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
        if (f.eof()) break;
    }
    if (!s.empty()) channel.push(s);
    channel.close();
    return 0;
}
int main() {
    // 1. Create a dummy test file
    std::ofstream out("messages.txt");
    out << "Do you have what it takes to be an engineer at TheStartup™?\n";
    out << "Are you willing to work 80 hours a week?\n";
    out << "Can you say \"synergy\" with a straight face?\n";
    out << "end"; // Note: no newline here to test the "leftover" logic
    out.close();

    // 2. Setup the channel and thread
    LineChannel channel;
    
    // Use std::ref to pass the channel by reference to the thread
    std::thread readerThread(readTCP, std::ref(channel));

    // 3. Consume the data
    std::string str;
    while (channel.pop(str)) {
        std::cout << "read: " << str << std::endl;
    }

    readerThread.join();
    std::cout << "--- Connection Closed ---" << std::endl;

    return 0;
}