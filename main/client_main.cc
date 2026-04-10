#include <unistd.h>

#include "client.h"

int main(int argc, char** argv) {
  Client c("127.0.0.1", 8080);
  for (int i = 0; i < 10; ++i) {
    Packet* p = new Packet(1, 10 - i, 25, 1);
    c.SendPacket(p);
  }

  sleep(3);
}
