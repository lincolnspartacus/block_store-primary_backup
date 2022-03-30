#include "primarybackup_client.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <sys/sendfile.h>

using grpc::ServerWriter;

PrimaryBackupRPCClient::PrimaryBackupRPCClient(std::shared_ptr<Channel> channel)
        : stub_(PrimaryBackupRPC::NewStub(channel))
{   
    return;
}

/*
 * Used by Primary --------> Backup  for replication
 */
int PrimaryBackupRPCClient::WriteBlock(const WriteRequest *request)
{
    ClientContext context;
    WriteResponse reply;
    Status status = stub_->WriteBlock(&context, *request, &reply);

    // Act upon its status.
    if (status.ok())
    {
        // Written to Backup successfully
        return 0;
    } else {
        // Return -1 if the Backup is dead.
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

int PrimaryBackupRPCClient::ReSync(const std::set<int64_t> &BlockSet)
{
    ClientContext context;
    WriteResponse response;
    WriteRequest request;

    std::unique_ptr <ClientWriter<WriteRequest>> writer(
                stub_->ReSync(&context, &response));
    for (auto it=BlockSet.begin(); it!=BlockSet.end(); ++it){
        request.set_address(*it * 4096);
        char buf[4096];
        memset(buf,0xff,4096);
        request.set_data(std::string(buf,buf+4096));
        writer->Write(request);
    }
    writer->WritesDone();
    Status status = writer->Finish();

    if(status.ok()){
        std::cout << "Write to Backup complete "<<std::endl;
        return 0;
    }
    else{
        return -1;
    }


}