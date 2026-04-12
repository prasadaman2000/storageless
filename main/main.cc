#include <unistd.h>

#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "app.h"

ABSL_FLAG(std::string, peer_ip, "", "IP address for peer");
ABSL_FLAG(int, peer_port, -1, "Port on peer");
ABSL_FLAG(int, server_port, -1, "Port to start server on");
ABSL_FLAG(std::string, commit_retrive_file, "",
          "file to commit and then retrieve");
ABSL_FLAG(std::string, out_file, "", "file to write retrieved file to");

/*
usage: ./app <peer-ip> <peer-port> <server-port> <commit-retrieve-file>
<out-file>
*/
int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);

  Application app(absl::GetFlag(FLAGS_peer_ip), absl::GetFlag(FLAGS_peer_port),
                  absl::GetFlag(FLAGS_server_port));

  app.CommitFile(absl::GetFlag(FLAGS_commit_retrive_file));

  sleep(10);

  app.RetrieveFile(absl::GetFlag(FLAGS_commit_retrive_file),
                   absl::GetFlag(FLAGS_out_file));

  while (true);
}
