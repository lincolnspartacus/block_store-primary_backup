#include "block_client.h"
#include "response_codes.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <sys/sendfile.h>


BlockRPCClient::BlockRPCClient(std::shared_ptr<Channel> channel)
        : stub_(BlockRPC::NewStub(channel))
{   
    ip_server1 = "localhost:50051";
    ip_server2 = "localhost:50052";
    return;
}

int BlockRPCClient::ReadBlock(int64_t address, uint8_t *buf)
{
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    ReadRequest req;
    req.set_address(address);
    ReadResponse reply;
    
    Status status = stub_->ReadBlock(&context, req, &reply);

    // Act upon its status.
    if (status.ok())
    {
        // TODO: Check response codes here! Redirect maybe?

        // Fill up the client's buffer with 4Kb data
        memcpy(buf, reply.data().c_str(), 4096);
        return 0;
    } else {
        // TODO: Retry here! Contact other server maybe?
    }

    return 0;
}

int BlockRPCClient::WriteBlock(int64_t address, uint8_t *buf)
{
     // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    WriteRequest req;
    req.set_address(address);
    req.set_data(std::string(buf, buf + 4096));

    WriteResponse reply;
    
    Status status = stub_->WriteBlock(&context, req, &reply);

    // Act upon its status.
    if (status.ok())
    {
        // TODO: Check response codes here! Redirect maybe?
        return 0;
    } else {
        // TODO: Retry here! Contact other server maybe?
    }

    return 0;
}

int BlockRPCClient::DoMessageInt(int in)
{
    // Data we are sending to the server.
    MessageInt req;
    req.set_value(in);

    // Container for the data we expect from the server.
    MessageInt reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    //auto start = std::chrono::high_resolution_clock::now();
    Status status = stub_->DoMessageInt(&context, req, &reply);
    //auto end = std::chrono::high_resolution_clock::now();
    //auto elapsed_seconds = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    //rtt.push_back(elapsed_seconds);

    // Act upon its status.
    if (status.ok())
    {
        return reply.value();
    }
    else
    {
        std::cout << status.error_code() << ": " << status.error_message()
                    << std::endl;
        return -1;
    }
}

