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

class BlockRPCServiceImpl final : public BlockRPC::Service
{
private:
    BlockRPCServiceImpl() { };
    std::string mFileStore;
public:
    BlockRPCServiceImpl(const std::string& fileStore);
    const string &getFileStorePath() const;
    void setFileStorePath(const string &fileStorePath);

protected:
    Status DoMessageInt(ServerContext *context, const MessageInt *request
                                    ,MessageInt *reply) override;
};
#endif
