#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <strstream>
#include <fstream>
#include <cassert>

#include "opencl_backend.hpp"

namespace detail {
    std::string getPlatformName(cl_platform_id id) {
        size_t size = 0;
        clGetPlatformInfo (id, CL_PLATFORM_NAME, 0, nullptr, &size);

        std::string result;
        result.resize (size);
        clGetPlatformInfo (id, CL_PLATFORM_NAME, size,
            const_cast<char*> (result.data ()), nullptr);

        return result;
    }

    std::string getDeviceName(cl_device_id id) {
        size_t size = 0;
        clGetDeviceInfo (id, CL_DEVICE_NAME, 0, nullptr, &size);

        std::string result;
        result.resize (size);
        clGetDeviceInfo (id, CL_DEVICE_NAME, size,
            const_cast<char*> (result.data ()), nullptr);

        return result;
    }

    void checkError(cl_int error) {
        if (error != CL_SUCCESS) {
            std::cerr << "OpenCL call failed with error " << error << std::endl;
            std::exit (1);
        }
    }

    std::string loadKernel (const char* name) {
        std::ifstream in (name);
        std::string result (
            (std::istreambuf_iterator<char> (in)),
            std::istreambuf_iterator<char> ());
        return result;
    }

    cl_program createProgram(const std::string& source, cl_context context) {
        // http://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clCreateProgramWithSource.html
        size_t lengths [1] = { source.size () };
        const char* sources [1] = { source.data () };

        cl_int error = 0;
        cl_program program = clCreateProgramWithSource (context, 1, sources, lengths, &error);
        checkError (error);

        return program;
    }

    cl_platform_id choosePlatform(bool quiet) {
        cl_uint platformIdCount = 0;
        clGetPlatformIDs (0, nullptr, &platformIdCount);

        if (platformIdCount == 0) {
            if (!quiet) std::cerr << "No OpenCL platform found" << std::endl;
            exit(-1);
        } else {
            if (!quiet) std::cerr << "Found " << platformIdCount << " platform(s)" << std::endl;
        }

        std::vector<cl_platform_id> platformIds (platformIdCount);
        clGetPlatformIDs (platformIdCount, platformIds.data (), nullptr);

        for (cl_uint i = 0; i < platformIdCount; ++i) {
            if (!quiet) std::cerr << "\t (" << i << ") : " << detail::getPlatformName(platformIds[i]) << std::endl;
        }

        if (platformIdCount > 1) {
            if (!quiet) std::cerr << "Using the first platform." << std::endl;
        }
        return platformIds[0];
    }

    std::pair<cl_device_id, cl_context> chooseDeviceAndCreateContext(
        cl_platform_id platform_id, bool quiet, int device_override
    ) {
        cl_uint deviceIdCount = 0;
        clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 0, nullptr, &deviceIdCount);

        if (deviceIdCount == 0) {
            if (!quiet) std::cerr << "No OpenCL devices found" << std::endl;
            exit(-1);
        } else {
            if (!quiet) std::cerr << "Found " << deviceIdCount << " device(s)" << std::endl;
        }

        std::vector<cl_device_id> deviceIds (deviceIdCount);
        clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, deviceIdCount, deviceIds.data(), nullptr);

        int selectedDeviceId = -1;
        if (!quiet || device_override < 0) {
            for (cl_uint i = 0; i < deviceIdCount; ++i) {
                std::cerr << "\t (" << i << ") : " << detail::getDeviceName(deviceIds[i]) << std::endl;
            }

            std::cerr << "Select one: ";
            std::cin >> selectedDeviceId;
        } else {
            selectedDeviceId = device_override;
        }

        assert(0 <= selectedDeviceId && selectedDeviceId < deviceIdCount);
        cl_device_id device_id = deviceIds[selectedDeviceId];

        const cl_context_properties contextProperties [] = {
            CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platform_id),
            0, 0
        };

        if (!quiet) std::cerr << "Creating context" << std::endl;
        cl_int error = CL_SUCCESS;
        cl_context context = clCreateContext(
            contextProperties,
            1, &device_id,
            nullptr, nullptr, &error);
        detail::checkError(error);

        return std::make_pair(device_id, context);
    }
};

opencl_backend::opencl_backend(size_t search_nonce_size, bool quiet, int device_override) {
    platform_id = detail::choosePlatform(quiet);
    std::pair<cl_device_id, cl_context> res =
        detail::chooseDeviceAndCreateContext(platform_id, quiet, device_override);
    device_id = res.first;
    context = res.second;

    if (!quiet) std::cerr << "Creating command queue" << std::endl;
    // http://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clCreateCommandQueue.html
    cl_int error = CL_SUCCESS;
    queue = clCreateCommandQueue(context, device_id, 0, &error);
    detail::checkError(error);
}

opencl_backend::~opencl_backend() {
    stop_search();
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}

char tohex(int i) {
    if (0 <= i && i < 10) return '0' + i;
    else if (10 <= i && i < 16) return 'A' + (i - 10);
    else assert(0);
}

void opencl_backend::start_search(
    size_t global_size,
    size_t local_size,
    size_t workset_size,
    uint8_t* block_data,
    uint8_t* target_hash
) {
    search_nonce = new search_nonce_kernel();

    search_nonce->global_size = global_size;
    search_nonce->local_size = local_size;
    search_nonce->workset_size = workset_size;

    std::cerr << "Creating program" << std::endl;
    // Create a program from source
    search_nonce->program = detail::createProgram(detail::loadKernel("kernels/kernel.cl"), context);

    std::ostringstream ss;
    for (size_t i = 0; i < 320; i+=4) {
        if (i == 0 || i == 4) continue;
        uint32_t value;
        if (i < 286) {
            value = *(uint32_t*)(block_data + i);
        } else {
            value = 0;
        }
        ss << "-DB" << (i / 64) << tohex((i % 64) / 4) << "=" << value << "U ";
    }

    for (size_t i = 0; i < 32; i += 8) {
        char j = (char)('A' + (3 - i / 8));
        ss << "-D" << j << "0=" << (*(uint64_t*)(target_hash + i)) << "UL ";
    }
    ss << "-DWORKSET_SIZE=" << workset_size;

    std::string options = ss.str();
    std::cerr << options << std::endl;

    std::cerr << "Building program" << std::endl;
    cl_int ret = clBuildProgram(
        search_nonce->program, 1, &device_id,
        options.data(), nullptr, nullptr);

    if (ret != 0) {
        size_t len = 0;
        clGetProgramBuildInfo(search_nonce->program, device_id, CL_PROGRAM_BUILD_LOG, NULL, NULL, &len);
        char *log = new char[len];
        clGetProgramBuildInfo(search_nonce->program, device_id, CL_PROGRAM_BUILD_LOG, len, log, NULL);
        std::cerr << "\n\nBuildlog:\n" << log << "\n\n";
        delete [] log;
        detail::checkError (ret);
    }

    std::cerr << "Creating search_nonce kernel" << std::endl;
    cl_int error;
    search_nonce->kernel = clCreateKernel(search_nonce->program, "search_nonce", &error);
    detail::checkError(error);

    std::cerr << "Preparing search_nonce buffers" << std::endl;
    search_nonce->result_buffer = clCreateBuffer(
        context, CL_MEM_WRITE_ONLY,
        8, nullptr, &error);
    detail::checkError(error);

    std::cerr << "Setting search_nonce arguments" << std::endl;
    clSetKernelArg(search_nonce->kernel, 1, sizeof(cl_mem), &search_nonce->result_buffer);
}
uint64_t opencl_backend::continue_search(uint64_t nonce) {
    clSetKernelArg(search_nonce->kernel, 0, 8, &nonce);

    uint64_t res = 0;

    detail::checkError(clEnqueueWriteBuffer(
        queue,
        search_nonce->result_buffer,
        true,    /* blocking_write */
        0,       /* offset */
        8,   /* size */
        &res,     /* ptr */
        0, nullptr, nullptr));

    // std::cerr << "Running the kernel" << std::endl;

    size_t offset[1] = {0};
    size_t size[1]   = {search_nonce->global_size};
    size_t local[1]  = {search_nonce->local_size};
    detail::checkError(
        clEnqueueNDRangeKernel(
            queue, search_nonce->kernel,
            1, offset, size, local,
            0, nullptr, nullptr));

    detail::checkError(clEnqueueReadBuffer(
        queue,
        search_nonce->result_buffer,
        true,                   /* blocking_read */
        0,                      /* offset */
        8, /* size */
        &res,             /* ptr */
        0, nullptr, nullptr));
    return res;
}
void opencl_backend::stop_search() {
    if (search_nonce != nullptr) {
        clReleaseMemObject(search_nonce->result_buffer);
        clReleaseKernel(search_nonce->kernel);
        clReleaseProgram(search_nonce->program);
    }
}