#include "common.h"

#include <cassert>

uint8_t hexchar2int(char c) {
    if ('0' <= c && c <= '9') c -= '0';
    else if ('a' <= c && c <= 'f') c = c - 'a' + 10;
    else assert(false);
    return c;
}

int compare_uint256(const void* first, const void* second) {
    for (int i = 0; i < 4; i++) {
        uint64_t h1 = ((uint64_t*) first)[3 - i];
        uint64_t h2 = ((uint64_t*) second)[3 - i];
        if (h1 > h2) {
            return 1;
            break;
        } else if (h1 == h2) continue;
        else return -1;
    }
    return 0;
}