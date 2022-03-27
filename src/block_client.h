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

private:
    std::unique_ptr<BlockRPC::Stub> stub_;
};


#endif
