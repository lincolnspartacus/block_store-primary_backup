# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(RBS)
file(GLOB SRCS_SERVER server.cc locks.cpp block_server.cpp state_machine.cpp primarybackup_server.cpp primarybackup_client.cpp)
file(GLOB SRCS_CLIENT client.cpp block_client.cpp client_library.cpp)

add_executable(primary ${SRCS_SERVER})
add_executable(backup ${SRCS_SERVER})
add_executable(client ${SRCS_CLIENT})

target_link_libraries(primary
        Threads::Threads
        gRPC::grpc++
        p3protolib)
add_dependencies(primary p3protolib)
set_target_properties(primary PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
target_compile_options(primary PRIVATE -DDEFAULT_ROLE=STATE_PRIMARY -DSELF_IP="localhost:50052" -DOTHER_IP="localhost:50053")

target_link_libraries(backup
        Threads::Threads
        gRPC::grpc++
        p3protolib)
add_dependencies(backup p3protolib)
set_target_properties(backup PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
target_compile_options(backup PRIVATE -DDEFAULT_ROLE=STATE_BACKUP -DSELF_IP="localhost:50053" -DOTHER_IP="localhost:50052")

target_link_libraries(client
        Threads::Threads
        gRPC::grpc++
        p3protolib)
add_dependencies(client p3protolib)
set_target_properties(client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

# set(CMAKE_BUILD_TYPE Debug)
# cmake -DCMAKE_BUILD_TYPE=Debug
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -D_FILE_OFFSET_BITS=64")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall --pedantic -g")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
