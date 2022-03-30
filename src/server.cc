#include "block_server.h"
#include "primarybackup_server.h"
#include "primarybackup_client.h"
#include "state_machine.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string>

PrimaryBackupRPCClient *g_RPCCLient; // gRPC handle to call RPCs in the other server

void sigintHandler(int sig_num)
{
    std::cerr << "Clean Shutdown\n";
    //    if (srv_ptr) {
    //        delete srv_ptr;
    //    }
    fflush(stdout);
    std::exit(0);
}

void mountdir(char* root, long size){
    
    //256 GB
    unsigned long long int bytes = 1024;
    unsigned long long int X;
    X = bytes*bytes*bytes*size - 1;
    //unsigned long long int X =  274877906943;
    FILE *fp = fopen("/mnt/sda4/data", "w");
    fseek(fp, X , SEEK_SET);
    fputc('\0', fp);
    fclose(fp);
    printf("Filesystem mounted on %s\n",root);
}

void run_server(char* loc)
{
    std::string mountpoint(loc);
    std::string server_address;
    if(DEFAULT_ROLE == STATE_PRIMARY)
        server_address = "localhost:50050";
    else
        server_address = "localhost:50051";
    BlockRPCServiceImpl service(mountpoint);
    ServerBuilder builder;
    
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.SetMaxSendMessageSize(INT_MAX);
    builder.SetMaxReceiveMessageSize(INT_MAX);
    builder.SetMaxMessageSize(INT_MAX);

    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening for clients on " << server_address << std::endl;

    // gRPC between Primary-Backup
    PrimaryBackupRPCServiceImpl service2;
    ServerBuilder builder2;
    builder2.AddListeningPort(SELF_IP, grpc::InsecureServerCredentials());
    builder2.RegisterService(&service2);
    std::unique_ptr<Server> server2(builder2.BuildAndStart());
    std::cout << "Primary-Backup comm on " << SELF_IP << std::endl;

    grpc::ChannelArguments ch_args;
    ch_args.SetMaxReceiveMessageSize(INT_MAX);
    ch_args.SetMaxSendMessageSize(INT_MAX);
    g_RPCCLient = new PrimaryBackupRPCClient(grpc::CreateCustomChannel(OTHER_IP, grpc::InsecureChannelCredentials() , ch_args ));

    StateMachine::initState(g_RPCCLient);

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
    server2->Wait();
}

int main(int argc, char* argv[])
{
    char* mountpoint;
    mountpoint = strdup("/mnt/sda4/data");
    if(argc<=1){
        std::cout<<"Usage: $./server 1  for New Block Storage OR $./server 0  for existing block storage "<<std::endl;
        exit(0);
    }
    if (argc>1){
        if (argv[1]=='1'){
            long size = 256;
            if (argc==3){

                char *p;
                size = strtol(argv[2], &p, 10);
            }
            mountdir(mountpoint,size)
        }
    }
    

    signal(SIGINT, sigintHandler);
    run_server(mountpoint);
}
