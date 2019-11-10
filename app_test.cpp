#include <string>
#include <cstdio>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <chrono>
#include <functional>

#include "blake2s_ref.h"

#include "common.h"
#include "opencl_backend.hpp"

const uint8_t block1[64] = {
    254, 186, 234, 85, 150,
    217, 218, 221, 53, 239,
    228, 248, 22, 187, 227,
    143, 27, 37, 51, 25,
    110, 92, 229, 182, 145,
    205, 46, 84, 5, 28,
    212, 213, 231, 26, 113,
    233, 238, 58, 3, 77,
    159, 110, 205, 169, 18,
    210, 158, 130, 3, 59,
    95, 179, 141, 94, 95,
    248, 39, 18, 64, 210,
    47, 73, 4, 97
};

const uint8_t block2[64] = {
    110, 141, 252, 145, 158,
    4, 250, 113, 41, 212,
    18, 172, 251, 169, 90,
    199, 211, 226, 253, 92,
    195, 215, 161, 253, 100,
    207, 63, 73, 77, 100,
    95, 93, 167, 17, 86,
    129, 51, 208, 236, 251,
    229, 216, 101, 186, 235,
    157, 150, 127, 241, 197,
    140, 41, 20, 79, 144,
    215, 123, 153, 132, 71,
    211, 154, 83, 231
};

void ref_blake2s_run320(uint64_t* nonce, uint8_t buf[320-8], uint8_t hash[32], size_t last_block_size) {
    assert(1 <= last_block_size && last_block_size <= 64);
    blake2s_state state;
    blake2s_init(&state, BLAKE2S_OUTBYTES);
    blake2s_update(&state, nonce, 8);
    blake2s_update(&state, buf, 320 - 8 - 64 + last_block_size);
    blake2s_final(&state, hash, BLAKE2S_OUTBYTES);
}

void ref_search_nonce(
  size_t gid,
  uint64_t* start_nonce,
  uint64_t work_set,
  uint8_t* buf,
  size_t last_block_size,
  uint8_t* target_hash,
  uint8_t* result_ptr
) {
    assert(1 <= last_block_size && last_block_size <= 64);
    uint64_t nonce0 = *start_nonce + gid * work_set;

    for (uint64_t i = 0; i < work_set; i++) {
        uint64_t nonce = nonce0 + i;

        blake2s_state state;
        uint8_t hash[32];
        blake2s_init(&state, BLAKE2S_OUTBYTES);
        blake2s_update(&state, &nonce, 8);
        blake2s_update(&state, buf, 320-8-64+last_block_size);
        blake2s_final(&state, hash, BLAKE2S_OUTBYTES);

        result_ptr[gid * work_set + i] = compare_uint256(target_hash, hash);
    }
}

#if defined(__cplusplus)
extern "C" {
#endif
void blake2s_run320(uint64_t* nonce, uint8_t buf[320-8], uint8_t hash[32]);
void search_nonce(
  size_t gid,
  uint64_t* start_nonce,
  uint64_t work_set,
  uint8_t* buf,
  uint8_t* target_hash,
  uint8_t* result_ptr
);
#if defined(__cplusplus)
}
#endif

int test_run320() {

}

int app_test(int argc, const char * const * argv) {
    // minerboi test <ref|cpu|opencl>
    assert(argc == 3);
    std::string tpe = std::string(argv[2]);

    std::function<void(uint64_t, uint8_t*, uint8_t*)> ref_start;

    uint64_t nonce = 0x32;
    uint8_t test_hash[BLAKE2S_OUTBYTES];

    uint8_t buf[320] = { 0x7a,0xa4,0xb0,0x9,0x8f,0x36,0xbf,0x2b,0x6e,0x23,0xd5,0x16,0x91,0xc9,0x40,0x59,0xd5,0x71,0x46,0xb8,0x63,0xfe,0xf8,0x6f,0x37,0x47,0x33,0x5e,0x6d,0xff,0xae,0x62,0x38,0x5d,0x75,0x45,0xaa,0x98,0xd8,0x95,0x2c,0x55,0xac,0xa1,0xdc,0x60,0xbb,0xff,0x7d,0xb8,0xb4,0xbb,0x94,0xd0,0x6c,0x8a,0x44,0xcf,0xf0,0xf,0x31,0x12,0x10,0x35,0x2f,0x98,0xbd,0xee,0x25,0x45,0x85,0xb2,0xdb,0x2a,0x47,0xf,0x29,0x62,0xa9,0x7e,0x6e,0x3b,0x6d,0x29,0x43,0x29,0x75,0x69,0x77,0x11,0x9a,0x4e,0x74,0xe6,0x8e,0x6f,0xc1,0x95,0xba,0x2b,0x81,0xdb,0xfb,0x77,0x98,0xc1,0x15,0x1d,0x76,0xb2,0xae,0x71,0x44,0x8e,0x7c,0x61,0xa7,0xbd,0xd8,0x24,0xfc,0xe,0x89,0xca,0x3e,0xbe,0x37,0x89,0x7f,0xe3,0x5d,0x46,0x74,0x4b,0xcc,0xd6,0xa6,0xdd,0xf9,0x56,0xea,0xd8,0x7a,0x4c,0xd8,0x8d,0x6c,0x15,0x3f,0x48,0x8f,0x6a,0x2f,0x95,0xe6,0xa4,0xc1,0xe7,0xbc,0x86,0x9,0xda,0,0x16,0x22,0x27,0x1a,0xcc,0x71,0xb7,0x5f,0xf5,0xe5,0x3a,0x28,0xf3,0x18,0x7a,0xd3,0x8e,0x5a,0xee,0x33,0x66,0x87,0x80,0xb1,0x6,0x71,0xbc,0x32,0x45,0xc8,0xc1,0x8c,0x20,0xbd,0x8d,0x3f,0x97,0x43,0x54,0x6f,0x97,0x4d,0xb6,0x8c,0x33,0x92,0xbc,0x29,0xe2,0x52,0x74,0x42,0xb2,0,0xa7,0x7e,0x78,0xca,0x1,0xc4,0xa2,0x97,0xfa,0xa7,0xa2,0xb4,0x94,0x63,0xd,0x35,0x52,0x39,0x1d,0x37,0xf6,0x14,0x4a,0x59,0xa8,0xf3,0x73,0x1f,0x49,0x80,0xb5,0x8f,0xb1,0xce,0xc9,0xd6,0x7c,0x7,0x9c,0x5c,0x22,0x30,0xf1,0x5d,0xc4,0x3e,0x21,0xfa,0xec,0x19,0xd3,0x1e,0xe2,0x26,0x56,0x4b,0xf2,0x96,0x16,0x81,0x1c,0x51,0xa3,0xf6,0x1,0x89,0x7e,0xa9,0xe};
    size_t bufsize = 286;
    uint32_t last_block_size = bufsize - (320-64);
    memset(buf + 256 + last_block_size, 0, 64 - last_block_size);

    uint8_t target_hash[BLAKE2S_OUTBYTES] = {
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
        0xff,0xff,0xff,0xff,0xff,0xff,0,0};

    size_t global_size = 1024 * 1024;
    uint8_t* results = new uint8_t[global_size];

    auto t_start = std::chrono::high_resolution_clock::now();
    if (tpe == "ref") {
        // 165, 140, 28, 216, 245, 110, 206, 21, 25, 163, 80, 187, 79, 202, 107, 171,
        // 83, 184, 235, 168, 143, 79, 126, 152, 159, 241, 28, 207, 111, 119, 184, 20,
        ref_blake2s_run320(&nonce, buf + 8, test_hash, last_block_size);
        for (int i = 0; i < BLAKE2S_OUTBYTES; i++) {
            printf("%d, ", test_hash[i]);
        }
        printf("\n");

        ref_search_nonce(0, &nonce, global_size, buf + 8, last_block_size, target_hash, results);
        for (int i = 0; i < 1024; i++) {
            printf("%d;", results[i]);
        }
        printf("\n");

    } else if (tpe == "cpu") {
        assert(false);

        // blake2s_run320(&nonce, buf + 8, test_hash);
        // for (int i = 0; i < BLAKE2S_OUTBYTES; i++) {
        //     printf("%d, ", test_hash[i]);
        // }
        // printf("\n");

        // ref_search_nonce(0, &nonce, 1024 * 1024, buf + 8, target_hash, results);
        // for (int i = 0; i < 1024; i++) {
        //     printf("%d;", results[i]);
        // }
        // printf("\n");
    } else if (tpe == "opencl") {
        uint8_t* results1 = new uint8_t[global_size];

        ref_search_nonce(0, &nonce, global_size, buf + 8, last_block_size, target_hash, results1);

        opencl_backend backend(global_size, false, -1);

        // backend.do_run320(&nonce, buf + 8, last_block_size, test_hash);
        // for (int i = 0; i < BLAKE2S_OUTBYTES; i++) {
        //     printf("%d, ", test_hash[i]);
        // }
        // printf("\n");

        // backend.do_search_nonce(
        //     global_size, 256,
        //     nonce, 1, buf + 8, last_block_size, target_hash, results);

        // for (int i = 0; i < global_size; i++) {
        //     assert(results[i] == results1[i]);
        // }

        // for (int i = 0; i < 1024; i++) {
        //     printf("%d;", results[i]);
        // }
        // printf("\n");
    }

    auto t_end = std::chrono::high_resolution_clock::now();
    float milliseconds =  std::chrono::duration<double, std::milli>(t_end-t_start).count();
    printf("seconds = %.2f\n", milliseconds / 1000.0);

    /**
     * auto t_start = std::chrono::high_resolution_clock::now();

        int times = 10;
        for (int i = 0; i < times; i++) {
            std::cout << "Running!" << std::endl;
        }

        auto t_end = std::chrono::high_resolution_clock::now();
        float milliseconds =  std::chrono::duration<double, std::milli>(t_end-t_start).count();
        std::cout << "seconds = " << (milliseconds / 1000.0) << std::endl;
        std::cout << "rate = " << (times * total_size / 1.0e6 / (milliseconds / 1000.0)) << " MH/s" << std::endl;

    */
}