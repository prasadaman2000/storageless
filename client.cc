#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h> // memset
#include <arpa/inet.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include "client.h"

inline constexpr int kMaxNumRetries = 4;

/*
 * client_connect takes a port number and establishes a connection as a client.
 * connectport: port number of server to connect to
 * returns: valid socket if successful, -1 otherwise
 */
Client::Client(std::string ip_addr, int connectport) {
    int connfd;
    struct sockaddr_in servaddr;

    connfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connfd < 0) {
        std::cerr << "FATAL: could not create client socket.";
        exit(1);
    }
    memset(&servaddr, 0, sizeof servaddr);

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(connectport);

    inet_pton(AF_INET, ip_addr.c_str(), &(servaddr.sin_addr));

    int retry_count = 0;

    while (retry_count < kMaxNumRetries) {
        if (connect(connfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
            std::cerr << "WARNING: could not connect to " << ip_addr << ":" << connectport << " will retry\n";
            sleep(10);
            ++retry_count;
        }
        else {
            break;
        }
    }

    if (retry_count == kMaxNumRetries) {
        std::cerr << "FATAL: could not connect to " << ip_addr << ":" << connectport << ". Exiting...\n";
        exit(1);
    }

    client_fd_ = connfd;

    worker_thread_ = std::thread([this]() {
        listen();
        });
}

void Client::Shutdown() {
    die = true;
    cv_.notify_all();
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
}

Client::~Client() {
    Shutdown();
}

bool Client::SendPacket(Packet* packet) {
    {
        std::unique_lock<std::mutex> lock(mu_);
        work_queue_.push_front(packet);
    }
    cv_.notify_one();
    return true;
}

void Client::listen() {
    Packet* p;
    while (!die) {
        {
            std::unique_lock<std::mutex> lock(mu_);
            cv_.wait(lock, [this] { return die || !work_queue_.empty();});
            if (work_queue_.empty()) {
                continue;
            }
            p = work_queue_.back();
            work_queue_.pop_back();
        }
        int sent_size = send(client_fd_, p, sizeof(Packet), 0);
        // std::cout << "Sent packet with " << p->chunk_size << " bytes.\n";
        delete p;
    }
}
