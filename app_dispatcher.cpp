#include <cstdio>
#include <cstring>
#include <cassert>
#include <cstdint>
#include <inttypes.h>
#include "blake2s_ref.h"

#include "common.h"
#include "opencl_backend.hpp"

void read_target_bytes(const char* str, uint8_t* target) {
    assert(strlen(str) == 64);
    for (size_t i = 0; i < 32; i++) {
        uint8_t c1 = hexchar2int(str[2 * i]);
        uint8_t c2 = hexchar2int(str[2 * i + 1]);
        target[i] = (c1 << 4) | c2;
    }
}

int app_dispatcher(int argc, const char * const * argv) {
    // minerboi dispatcher <fifo_out> <fifo_in> <hash>

    // Takes 2 FIFO ports and the target hash as arguments.
    assert(argc == 5);
    const char* fifo_out_path = argv[2];
    const char* fifo_in_path  = argv[3];

    // The target HASH.
    uint8_t target_hash[32];
    read_target_bytes(argv[4], target_hash);

    // Input bytes.
    const size_t BUF_SIZE = 4 * 1024;
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    size_t bufsize = fread(buf, 1, BUF_SIZE, stdin);
    assert(bufsize >= 8);
    assert(bufsize < BUF_SIZE);

    fprintf(stderr, "bufsize = %d\n", bufsize);

    assert(320 - 64 + 1 <= bufsize && bufsize <= 320);

    fprintf(stderr, "Sending work off...\n");

    FILE* fifo_out = fopen(fifo_out_path, "wb");

    fwrite(target_hash, 1, 32, fifo_out);
    uint32_t bufsize_uint32 = (uint32_t) bufsize;
    fwrite(&bufsize_uint32, 1, 4, fifo_out);
    fwrite(buf, 1, bufsize, fifo_out);
    fclose(fifo_out);

    fprintf(stderr, "Waiting for nonce.\n");

    FILE* fifo_in  = fopen(fifo_in_path, "rb");
    uint64_t nonce = 0x0;
    int c = fread(&nonce, 1, 8, fifo_in);
    fclose(fifo_in);
    assert(c == 8);
    printf("%016" PRIx64, nonce);

    return 0;
}