#include<bits/stdc++.h>
// reads from a file or a connection 8 bytes at a time , only ouptuts 
//after encountering a new line '/n'

class LineChannel{
    std::queue<std::string> q;
    std::condition_variable cv;
    std::mutex mtx;
    bool closed=false;// represents closed or open channel
    public :

    void push(std::string &s){
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock);
        q.push(s);
        cv.notify_all();
    }
    
    void pop(std::string &out){
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock,[this]{return q.size()!=0|| closed;});
        out=q.front();
        q.pop();
        cv.notify_all(); 
    }
    bool close(){
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock);
        closed = true; // close the channel after reading all the data 
        cv.notify_all();
        return true;
    }
};

  int readTCP(LineChannel & channel){
    std::ifstream f("messages.txt");// simulating connection , replace with socket later 
    char buffer[8];
    std::string s;
    while(true){
        f.read(buffer,8);
        std::streamsize sz=f.gcount();
        if(sz>0){
           for(auto i=0;i<sz;i++){
              if(buffer[i]!='\n') s.push_back(buffer[i]);
              else {
                channel.push(s);
                s.clear();
              }
           }
        }
        if(f.eof()) break;
        if(f.fail()) return -1;
    }
    if(s.size()>0) channel.push(s);
    channel.close();
}

