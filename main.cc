#include "app.h"

#include <unistd.h>

int main(int argc, char** argv) {
    int peer_port = atoi(argv[2]);
    int server_port = atoi(argv[3]);

    Application app(argv[1], peer_port, server_port);

    app.CommitFile("server.cc");
    app.CommitFile("app.cc");

    sleep(10);

    app.RetrieveFile("server.cc", "server_copy.cc");

    while (true);
}
