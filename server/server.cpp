#include "server.hpp"

void server::readerAndWriterForOne(std::unique_ptr<LineChannel> channel, int client_sck_id){
    std::thread reader ([&](){readTCP(*channel,client_sck_id);}); //
    // have a consumer , currently writes to a file maybe we provide to some sort of interface that basically processes and responds 
    std::string randomFN{"output"+client_sck_id};
    std::ofstream f(randomFN);
    std::string line;
    while((*channel).pop(line)){
         f << line << std::endl;// ideally  hr ,everything seperate , process it and send some response i guess
    }

    reader.join();
    sem_post(&sm);// increase the amount of threads as this is done 
}


int server::listener(uint16_t port) {

    sem_init(&sm,0,5);
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
        sem_wait(&sm);
        int addrlen=sizeof(address);
        int client_fd=accept(server_fd,(struct sockaddr*)&address,(socklen_t*)&addrlen);
        // create a diffrent thread safe line channel for this thread 
        std::unique_ptr<LineChannel> channel=std::make_unique<LineChannel>();
        std::thread readerForOneConnection(readerAndWriterForOne,std::move(channel),client_fd);
        readerForOneConnection.detach();
    }
    close(server_fd);
    open=true;
    return 0;
}


