#ifndef client_h
#define client_h

#include <string>
#include <thread>
#include <mutex>
#include <list>
#include <condition_variable>

#include "packet.h"

class Client {
public:
    Client(std::string ip_addr, int port);
    ~Client();
    Client() = delete;
    bool SendPacket(Packet*);
    void Shutdown();

private:
    void listen();

    int client_fd_;
    std::thread worker_thread_;
    std::mutex mu_;
    std::condition_variable cv_;
    std::list<Packet*> work_queue_;
    bool die = false;
};

#endif
