#include <cstdio>
#include <cassert>
#include <cstring>
#include <inttypes.h>

#include <ctime>
#include <chrono>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "blake2s_ref.h"

int app_test(int argc, const char * const * argv);
int app_worker(int argc, const char * const * argv);
int app_dispatcher(int argc, const char * const * argv);

int main(int argc, const char* const* argv) {
    // minerboi <worker|dispatcher|test> ...

    assert(argc >= 2);
    std::string command = std::string(argv[1]);
    if (command == "test") app_test(argc, argv);
    else if (command == "worker") app_worker(argc, argv);
    else if (command == "dispatcher") app_dispatcher(argc, argv);
    else assert(false);
}