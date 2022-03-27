cmake_minimum_required(VERSION 3.10)


# CMakeLists.txt
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_BUILD_TYPE Release)
set (CMAKE_MAKE_PROGRAM make CACHE FILEPATH "" FORCE)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(RBS)

find_package(gRPC CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(protobuf CONFIG REQUIRED)


add_subdirectory(external)
add_subdirectory(src)


