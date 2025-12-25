#pragma once
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <condition_variable>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <map>
#include <thread>
#include <utility>
struct HttpReq{
    std::string requestLine;
    std::map<std::string,std::string> headers;
    std::string body;
};
// reads from a file or a connection 8 bytes at a time , only ouptuts
// after encountering a new line '/n'

class LineChannel { // glorious thread safe queue for safely writing to a queue and removing stuff from it 
    std::queue<std::string> q;// critical section ,(1 reader, 1 writer is usually enough for one connection)
    std::condition_variable cv;
    std::mutex mtx;
    bool closed = false;

public:
    void push(std::string s) {
        std::lock_guard<std::mutex> lock(mtx); // only wait when u have to wait
        q.push(std::move(s));
        cv.notify_one(); // notify one of the sleeping locks on pop
    }

    bool pop(std::string &out) {
        std::unique_lock<std::mutex> lock(mtx);
        // Wait until there is a message OR the channel is closed
        cv.wait(lock, [this] { return !q.empty() || closed; }); // avoids busy waiting

        // If the queue is empty and closed is true, we are done else popping can be done
        if (q.empty() && closed) return false;

        out = std::move(q.front());
        q.pop();
        return true;
    }

    void close() {
        std::lock_guard<std::mutex> lock(mtx);
        closed = true;
        cv.notify_all(); // Wake up any threads waiting in pop()
    }

    bool clearChannel() {
        std::lock_guard<std::mutex> lock(mtx);
        std::queue<std::string> empty;
        swap(q, empty);
        closed = false;
        cv.notify_all();
        return true;
    }
};

int readTCP(LineChannel &channel, int sck_fd,HttpReq &hr); // sck_fd==socket file descriptor

