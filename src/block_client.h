#ifndef RBS_BLOCK_CLIENT_H
#define RBS_BLOCK_CLIENT_H

#include "rbs.grpc.pb.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <signal.h>
#include <chrono>
#include <ctime>
#include <fstream>
#include <memory>
#include <sys/stat.h>
#include <time.h>

using namespace grpc;
using namespace rbs;

class BlockRPCClient
{

public:
    BlockRPCClient(std::shared_ptr<Channel> channel);

    int DoMessageInt(int in);
    int ReadBlock(int64_t address, uint8_t *buf);
    int WriteBlock(int64_t address, uint8_t *buf);

private:
    std::unique_ptr<BlockRPC::Stub> stub_;
    std::string ip_server1;
    std::string ip_server2;
};


#endif
