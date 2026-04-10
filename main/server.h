#ifndef server_h
#define server_h

#include <thread>
#include <list>
#include <mutex>
#include <optional>

#include "packet.h"

class Server {
public:
    Server(int port);
    ~Server();
    Server() = delete;
    void start();
    Packet* PopFromQueue();
    Packet* NextPacket();
    void Shutdown();

private:
    int server_fd_;
    int client_socket_;
    std::thread listen_thread_;
    std::list<Packet*> work_queue_;
    bool die_ = false;

    std::mutex q_tex_;
};

#endif
