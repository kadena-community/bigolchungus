CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(minerboi)
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

FIND_PACKAGE(OpenCL REQUIRED)
INCLUDE_DIRECTORIES(${OPENCL_INCLUDE_DIR})

#ADD_EXECUTABLE(minerboi main.cpp
#                        app_test.cpp
#                        app_dispatcher.cpp app_worker.cpp
#                        common.cpp blake2s_ref.c fast_blake.c
#                        opencl_backend.cpp)
#TARGET_LINK_LIBRARIES(minerboi ${OPENCL_LIBRARY})

ADD_EXECUTABLE(test_opencl test_opencl.cpp common.cpp blake2s_ref.c fast_blake.c opencl_backend.cpp)
TARGET_LINK_LIBRARIES(test_opencl ${OPENCL_LIBRARY})