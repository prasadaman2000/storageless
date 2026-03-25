#ifndef app_h
#define app_h

#include <string>
#include <map>
#include <set>
#include <memory>
#include <iostream>

#include "server.h"
#include "client.h"

class Application {
public:
    Application(std::string fwd_address, int fwd_port, int server_port);
    ~Application();

    bool CommitFile(std::string fname);
    bool RetrieveFile(std::string fname, std::string fout);

private:
    struct RetrieveRequest {
        std::unique_ptr<std::ofstream> stream;
        int last_seq_seen;
    };

    Server s_;
    Client c_;

    std::thread fwd_thread_;
    bool die_ = false;

    std::map<std::string, RetrieveRequest> retrieve_reqs_;
    std::set<std::string> seen_files_;
};

#endif