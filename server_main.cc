#include "server.h"

int main(int argc, char** argv) {
    Server s(8080);
    s.start();

    return 0;
}
