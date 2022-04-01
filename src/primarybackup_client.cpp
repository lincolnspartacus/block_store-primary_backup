#include "primarybackup_client.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <sys/sendfile.h>

using grpc::ServerWriter;
using grpc::ClientWriter;

PrimaryBackupRPCClient::PrimaryBackupRPCClient(std::shared_ptr<Channel> c)
        :channel(c), stub_(PrimaryBackupRPC::NewStub(channel))
{   
    return;
}

/*
 * Used by Primary --------> Backup  for replication
 */
int PrimaryBackupRPCClient::WriteBlock(const WriteRequest *original_request)
{
    ClientContext context;
    WriteRequest request;
    request.set_address(original_request->address());
    request.set_data(original_request->data());
    WriteResponse reply;
    Status status = stub_->WriteBlock(&context, request, &reply);

    // Act upon its status.
    if (status.ok())
    {
        // Written to Backup successfully
        std::cout << "[PrimaryBackupRPCClient::WriteBlock] Written to Backup!\n";
        return 0;
    } else {
        // Return -1 if the Backup is dead.
        std::cout << "[PrimaryBackupRPCClient::WriteBlock] Couldn't replicate WRITE\n";
        std::cout << "[PrimaryBackupRPCClient::WriteBlock]" << status.error_code() << ": " 
                  << status.error_message() << std::endl;
        return -1;
    }
}

int PrimaryBackupRPCClient::GetState(int in)
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
    Status status = stub_->GetState(&context, req, &reply);
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

int PrimaryBackupRPCClient::ReSync()
{
    ClientContext context;
    WriteRequest request;
    Empty emptyReq;

    std::unique_ptr <ClientReader<WriteRequest>> reader(stub_->ReSync(&context, emptyReq));

    std::cout << "[PrimaryBackupRPCClient::ReSync] Inside here "<<std::endl;

    while (reader->Read(&request)) {
        std::cout << "Address :  " << request.address() << "\n"; 
        const uint8_t *buf = (const uint8_t *)(request.data().c_str());
        for(int i = 0; i < 4096; ++i)
            printf("%x ", buf[i]);
        printf("\n");
    }
    Status status = reader->Finish();
    if(status.ok()){
        std::cout << "[PrimaryBackupRPCClient::ReSync] Resync finished! "<<std::endl;
        return 0;
    }
    else{
        std::cout << status.error_code() + ": " + status.error_message() << std::endl;
        return -1;
    }
}
