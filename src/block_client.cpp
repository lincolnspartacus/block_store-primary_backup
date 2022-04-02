#include "block_client.h"
#include "response_codes.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <sys/sendfile.h>


BlockRPCClient::BlockRPCClient(std::shared_ptr<Channel> channel)
        : stub_(BlockRPC::NewStub(channel)) {}

std::pair<Status, ReadResponse> BlockRPCClient::ReadBlock(int64_t address, uint8_t *buf)
{
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    ReadRequest req;
    req.set_address(address);
    ReadResponse reply;
    
    Status status = stub_->ReadBlock(&context, req, &reply);
    return {status, reply};
}

std::pair<Status, WriteResponse> BlockRPCClient::WriteBlock(int64_t address, uint8_t *buf)
{
     // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    WriteRequest req;
    req.set_address(address);
    req.set_data(std::string(buf, buf + 4096));

    WriteResponse reply;
    
    Status status = stub_->WriteBlock(&context, req, &reply);
    return {status, reply};
}