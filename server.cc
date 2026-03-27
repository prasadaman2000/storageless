// C++ program to show the example of server application in
// socket programming
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <mutex>

#include "server.h"
#include "packet.h"

Server::Server(int port) {
    // creating socket
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);

    // specifying the address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // binding socket.
    bind(server_fd_, (struct sockaddr*)&serverAddress,
        sizeof(serverAddress));

    // listening to the assigned socket
    if (listen(server_fd_, 5) == 0) {
        std::cout << "Listening on port " << port << "\n";
    }
    else {
        std::cout << "Failed to start listening on port " << port << "\n";
        exit(1);
    }
}

Packet* Server::NextPacket() {
    Packet* p = new Packet();
    // std::cout << "Waiting for packet\n";

    ssize_t bytes_read = 0;

    while (bytes_read < sizeof(Packet)) {
        ssize_t recvd = recv(client_socket_, p + bytes_read,
            sizeof(Packet) - bytes_read, 0);
        if (recvd < 0) {
            std::cout << "recvd = " << recvd << " with errno " << errno << "\n";
            continue;
        }
        // std::cout << "Read " << recvd << " bytes\n";
        bytes_read += recvd;
    }

    return p;
}

void Server::start() {
    listen_thread_ = std::thread([this]() {
        std::cout << "waiting! \n";
        client_socket_ = accept(server_fd_, nullptr, nullptr);
        std::cout << "peer connected on fd: " << client_socket_ << " and die=" << die_ << "\n";

        while (!die_) {
            Packet* p = NextPacket();
            if (p == nullptr) {
                std::cout << "Can't read packets from client anymore - exiting.\n";
                break;
            }

            // std::cout << "chunk_seq_num: " << p->chunk_seq_num << "\n";
            send(client_socket_, "chunk_seq_num: ", p->chunk_seq_num, 0);

            {
                std::lock_guard<std::mutex> lock(q_tex_);
                work_queue_.push_back(p);
            }

        }
        std::cout << "DYING\n";
        });
}

Packet* Server::PopFromQueue() {
    Packet* p;
    {
        std::lock_guard<std::mutex> lock(q_tex_);
        if (work_queue_.size() == 0) {
            return nullptr;
        }

        p = work_queue_.front();
        work_queue_.pop_front();
    }

    return p;
}

void Server::Shutdown() {
    die_ = true;

    close(server_fd_);
    close(client_socket_);

    if (listen_thread_.joinable()) {
        listen_thread_.join();
    }
}

Server::~Server() {
    Shutdown();
}