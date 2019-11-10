#include <cstdio>
#include <cstring>
#include <cassert>
#include <cstdint>
#include <inttypes.h>

#include "blake2s_ref.h"
#include "common.h"
#include "opencl_backend.hpp"

int app_worker(int argc, const char * const * argv) {
    // minerboi worker <fifo_in> <fifo_out>

    // Takes 2 FIFO ports and the target hash as arguments.
    assert(argc == 4);
    const char* fifo_in_path   = argv[2];
    const char* fifo_out_path  = argv[3];

    size_t size = 1024 * 1024 * 16;
    opencl_backend backend(size, false, -1);

    uint8_t target_hash[32];
    uint8_t buf[320];
    uint8_t* result = new uint8_t[size];

    FILE* fifo_in  = fopen(fifo_in_path, "rb");
    FILE* fifo_out = fopen(fifo_out_path, "wb");

    while (true) {
        int c = fread(target_hash, 1, 32, fifo_in);
        assert(c == 32);
        uint32_t bufsize;
        c = fread(&bufsize, 1, 4, fifo_in);
        assert(c == 4);
        memset(buf, 0, 320);
        c = fread(buf, 1, bufsize, fifo_in);
        assert(c == bufsize);

        uint32_t last_block_size = bufsize - (320-64);

        printf("Got work! bufsize = %d, last_block_size=%d\n", bufsize, last_block_size);

        uint64_t nonce = 0;

        while (true) {
            printf("Trying %ld - %ld\n", nonce, nonce + size - 1);

            backend.do_search_nonce(size, 256,
                nonce, 1, buf + 8, last_block_size, target_hash, result);

            bool found = false;
            for (size_t i = 0; i < size; i++) {
                if (result[i] >= 0) {
                    found = true;
                    nonce = nonce + i;
                    break;
                }
            }

            if (found) {
                printf("Done!\n");

                blake2s_state state;
                uint8_t hash[32];
                blake2s_init(&state, BLAKE2S_OUTBYTES);
                blake2s_update(&state, &nonce, 8);
                blake2s_update(&state, buf + 8, bufsize-8);
                blake2s_final(&state, hash, BLAKE2S_OUTBYTES);

                if (compare_uint256(target_hash, hash) == -1) {
                    printf("Bad nonce!!!\n");
                    exit(-1);
                }

                fwrite(&nonce, 1, 8, fifo_out);
                fflush(fifo_out);
                break;
            } else {
                nonce += size;
            }
        }
    }
}