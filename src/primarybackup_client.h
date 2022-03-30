#ifndef PRIMARYBACKUP_CLIENT_H
#define PRIMARYBACKUP_CLIENT_H

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

class PrimaryBackupRPCClient
{

public:
    PrimaryBackupRPCClient(std::shared_ptr<Channel> channel);

    int GetState(int in);
    int WriteBlock(const WriteRequest *request);
    int ReSync(const std::set<int64_t> &BlockSet);

private:
    std::unique_ptr<PrimaryBackupRPC::Stub> stub_;
};


#endif
