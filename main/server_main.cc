#include "server.h"

int main(int argc, char** argv) {
  Server s(8080);
  s.start();
  while (true);

  return 0;
}
