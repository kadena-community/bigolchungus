#ifdef __APPLE__
    #define CL_SILENCE_DEPRECATION
    #include <OpenCL/opencl.h>
#else
    #include "CL/cl.h"
#endif

struct search_nonce_kernel {
    cl_program program;
    cl_kernel kernel;
    cl_mem result_buffer;
    size_t global_size;
    size_t local_size;
    size_t workset_size;
};

struct opencl_backend {
    cl_platform_id platform_id;
    cl_device_id device_id;
    cl_context context;
    cl_command_queue queue;
    char* kernel_path;

    search_nonce_kernel* search_nonce;

    opencl_backend(size_t search_nonce_size, bool quiet, int device_override, int platform_override, char* kernel_path_override);
    ~opencl_backend();

    void start_search(
        size_t global_size,
        size_t local_size,
        size_t workset_size,
        uint8_t* block_data,
        uint8_t* target_hash
    );
    uint64_t continue_search(uint64_t nonce);
    void stop_search();
};
