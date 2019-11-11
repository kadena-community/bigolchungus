#include <cstdio>
#include <cstring>
#include <cassert>
#include <cstdint>
#include <inttypes.h>
#include <chrono>
#include <sstream>
#include <string>

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

void ref_search_nonce(
  size_t gid,
  uint64_t start_nonce,
  uint64_t work_set,
  uint8_t* buf,
  uint32_t last_block_size,
  uint8_t* target_hash,
  uint8_t* result_ptr
) {
    uint64_t nonce0 = start_nonce + gid * work_set;

    for (uint64_t i = 0; i < work_set; i++) {
        uint64_t nonce = nonce0 + i;

        blake2s_state state;
        uint8_t hash[32];
        blake2s_init(&state, BLAKE2S_OUTBYTES);
        blake2s_update(&state, &nonce, 8);
        blake2s_update(&state, buf, (320 - 64 + last_block_size) - 8);
        blake2s_final(&state, hash, BLAKE2S_OUTBYTES);

        // fprintf(stderr, "%ld -> %d\n", gid * work_set + i, compare_uint256(target_hash, hash));
        result_ptr[gid * work_set + i] = compare_uint256(target_hash, hash);
    }
}

int main(int argc, const char * const * argv) {
    // test_opencl <hash>

    bool quiet = false;
    int deviceOverride = 0;

    if (!quiet) fprintf(stderr, "Started\n");

    // The target HASH.
    assert(argc == 2 || argc == 3);
    uint8_t target_hash[32];
    if (argc == 2) {
        read_target_bytes(argv[1], target_hash);
    } else {
        std::istringstream(std::string(argv[1])) >> deviceOverride;
        read_target_bytes(argv[2], target_hash);
    }

    if (!quiet) {
        fprintf(stderr, "hash = ");
        for (int i = 0; i < 32; i++) {
            fprintf(stderr, "%#x,", target_hash[i]);
        }
        fprintf(stderr, "\n");
    }

    if (!quiet) fprintf(stderr, "Reading buf\n");
    const size_t BUF_SIZE = 4 * 1024;
    uint8_t buf[BUF_SIZE];
    size_t bufsize = fread(buf, 1, BUF_SIZE, stdin);
    assert(bufsize >= 8);
    assert(bufsize < BUF_SIZE);

    if (!quiet) {
        fprintf(stderr, "hash = ");
        for (int i = 0; i < bufsize; i++) {
            fprintf(stderr, "%#x,", buf[i]);
        }
        fprintf(stderr, "\n");
    }

    if (!quiet) fprintf(stderr, "bufsize = %d\n", bufsize);

    assert(320 - 64 + 1 <= bufsize && bufsize <= 320);
    uint32_t last_block_size = bufsize - (320-64);
    memset(buf + 256 + last_block_size, 0, 64 - last_block_size);

    if (!quiet) fprintf(stderr, "last_block_size = %d\n", last_block_size);

    size_t global_size = 1024 * 1024 * 16;
    size_t local_size = 1024;
    size_t workset_size = 64;

    uint64_t nonce_step_size = global_size * workset_size;
    // uint8_t* result = new uint8_t[nonce_step_size * 64];

    uint64_t start_nonce = 0;
    FILE* urandom = fopen("/dev/urandom","rb");
    fread(&start_nonce, 1, 8, urandom);
    fclose(urandom);

    opencl_backend backend(nonce_step_size, true, deviceOverride);

    backend.start_search(
        global_size, local_size, workset_size,
        buf, target_hash);

    int steps = 0;
    auto t_start = std::chrono::high_resolution_clock::now();
    while (true) {
        if (!quiet) fprintf(stderr,
            "Trying %#lx - %#lx\n", start_nonce, start_nonce + nonce_step_size - 1);
        steps += 1;
        uint64_t found = backend.continue_search(start_nonce);

        if (found != 0) {
            if (!quiet) fprintf(stderr, "Done %#lx!\n", found);

            blake2s_state state;
            uint8_t hash[32];
            blake2s_init(&state, BLAKE2S_OUTBYTES);
            blake2s_update(&state, &found, 8);
            blake2s_update(&state, buf + 8, bufsize - 8);
            blake2s_final(&state, hash, BLAKE2S_OUTBYTES);

            if (compare_uint256(target_hash, hash) == -1) {
                fprintf(stderr, "Bad nonce!!!\n");
                exit(-1);
            }

            auto t_end = std::chrono::high_resolution_clock::now();
            float milliseconds = std::chrono::duration<double, std::milli>(t_end-t_start).count();
            uint64_t numHashes = steps * nonce_step_size;
            double rate = numHashes / (milliseconds / 1000.0);
            printf("%016" PRIx64 " %ld %ld", found, numHashes, (uint64_t) rate);
            break;
        } else {
            start_nonce += nonce_step_size;
        }
    }

    return 0;
}
