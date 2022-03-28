#ifndef RBS_BLOCK_SERVER_H
#define RBS_BLOCK_SERVER_H


#include "rbs.grpc.pb.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <signal.h>
#include <iostream>
#include <errno.h>

using namespace rbs;
using namespace grpc;
using namespace std;

class PrimaryBackupRPCServiceImpl final : public PrimaryBackupRPC::Service
{
private:
    PrimaryBackupRPCServiceImpl() { };
public:
    PrimaryBackupRPCServiceImpl(const std::string& fileStore);

protected:
    Status GetState(ServerContext *context, const MessageInt *request
                                    ,MessageInt *reply) override;
};
#endif