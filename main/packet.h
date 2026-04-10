#ifndef packet_h
#define packet_h

#define PACKET_MAX_CHUNK_SIZE 1024

struct Packet {
    int fname_size;
    char fname[1024];
    int chunk_seq_num;
    int total_chunks;
    int chunk_size;
    char chunk[PACKET_MAX_CHUNK_SIZE];

    Packet(int fname_size, int chunk_seq_num, int total_chunks, int chunk_size) :
        fname_size(fname_size),
        chunk_seq_num(chunk_seq_num),
        total_chunks(total_chunks),
        chunk_size(chunk_size) {
    }

    Packet() = default;
};

#endif