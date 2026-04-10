#include "app.h"

#include <math.h>
#include <string.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

Application::Application(std::string fwd_address, int fwd_port, int server_port)
    : s_(server_port), c_(fwd_address, fwd_port) {
  s_.start();
  fwd_thread_ = std::thread([this]() {
    while (!die_) {
      if (Packet* p = s_.PopFromQueue(); p != nullptr) {
        std::string packet_fname = std::string(p->fname, p->fname_size);
        if (seen_files_.find(packet_fname) == seen_files_.end()) {
          seen_files_.insert(packet_fname);
          std::cout << "Got new file from peer: " << packet_fname << "\n";
        }

        bool intercepted = false;
        if (auto iter =
                retrieve_reqs_.find(std::string(p->fname, p->fname_size));
            iter != retrieve_reqs_.end()) {
          if (p->chunk_seq_num == iter->second.last_seq_seen + 1) {
            intercepted = true;
            iter->second.stream->write(p->chunk, p->chunk_size);
            iter->second.last_seq_seen = p->chunk_seq_num;

            if (p->chunk_seq_num == p->total_chunks) {
              iter->second.stream->close();
              retrieve_reqs_.erase(iter);
              std::cout << "finished retrieving " << p->fname << "\n";
            }
          }
        }

        if (!intercepted) {
          c_.SendPacket(p);
        }
      }
    }
  });
}

Application::~Application() {
  die_ = true;
  fwd_thread_.join();
}

bool Application::CommitFile(std::string fname) {
  std::ifstream file(fname);
  std::string file_contents;

  if (file) {
    std::ostringstream content_stream;
    content_stream << file.rdbuf();
    file.close();

    file_contents = content_stream.str();
  } else {
    std::cerr << "Error: Could not open the file " << fname << std::endl;
  }

  int chunk_num = 1;
  int total_chunks = (file_contents.size() / PACKET_MAX_CHUNK_SIZE) + 1;
  for (int total_bytes = 0; total_bytes < file_contents.size();
       total_bytes += PACKET_MAX_CHUNK_SIZE) {
    Packet* p = new Packet();
    p->chunk_seq_num = chunk_num++;
    p->total_chunks = total_chunks;

    p->fname_size = fname.size();
    memcpy(p->fname, fname.c_str(), p->fname_size);
    // std::cout << "Wrote " << fname.size() << " bytes to " << p->fname <<
    // "\n";

    p->chunk_size = std::min((unsigned long)PACKET_MAX_CHUNK_SIZE,
                             file_contents.size() - total_bytes);
    memcpy(p->chunk, file_contents.c_str() + total_bytes, p->chunk_size);
    // std::cout << "Wrote " << p->chunk_size << " bytes to " << (void*)p->chunk
    // << "\n";

    c_.SendPacket(p);
  }

  return true;
}

bool Application::RetrieveFile(std::string fname, std::string fout) {
  std::unique_ptr<std::ofstream> outfile =
      std::make_unique<std::ofstream>(fout, std::ios::trunc);
  if (!outfile->is_open()) {
    std::cout << "Could not open fout: " << fout << "\n";
    return false;
  }

  retrieve_reqs_[fname] = Application::RetrieveRequest{
      .stream = std::move(outfile), .last_seq_seen = 0};
  return true;
}
