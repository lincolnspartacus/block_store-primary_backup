#ifndef PRIMARYBACKUP_SERVER_H
#define PRIMARYBACKUP_SERVER_H


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
public:
    PrimaryBackupRPCServiceImpl() {};

protected:
    Status GetState(ServerContext *context, const MessageInt *request
                                    ,MessageInt *reply) override;
    Status WriteBlock(ServerContext *context, const WriteRequest *request,
                                    WriteResponse *reply) override;
};
#endif