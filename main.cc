#include "app.h"

#include <unistd.h>
#include <iostream>


/*
usage: ./app <peer-ip> <peer-port> <server-port> <commit-retrieve-file> <out-file>
*/
int main(int argc, char** argv) {
    std::cout << "Here";

    int peer_port = atoi(argv[2]);
    int server_port = atoi(argv[3]);

    Application app(argv[1], peer_port, server_port);

    app.CommitFile(argv[4]);

    sleep(10);

    app.RetrieveFile(argv[4], argv[5]);

    while (true);
}
