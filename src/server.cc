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

void printChannelState();
extern std::set<int64_t> BlockSet;
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

void mountdir(std::string root, long size){
    
    //256 GB
    unsigned long long int bytes = 1024;
    unsigned long long int X;
    X = bytes*bytes*bytes*size - 1;
    FILE *fp;
    if (access(root.c_str(), F_OK) == 0)
    {
        // file exists, open in read/write mode
        fp = fopen(root.c_str(), "r+");
        std::cout <<"Mount dir exists"<<std::endl;
    }
    else{
        //file doesn't exist, create(w+) and read/write file
        fp = fopen(root.c_str(), "w+");
    }
    if(fp == NULL){
        std::cout << "File mount issue "<<std::endl;
        exit(0);
    }
    fseek(fp, X , SEEK_SET);
    fputc('\0', fp);
    fclose(fp);
    printf("Filesystem mounted on %s\n",root.c_str());
}

void run_server(std::string mountpoint)
{
    std::string server_address{"0.0.0.0:50051"};
    StateMachine::setState(STATE_START);
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
    printChannelState();
    server->Wait();
    server2->Wait();
}

int main(int argc, char* argv[])
{
    std::string default_mount_point{"/users/pandotra/tmp/data"};
    //mountpoint = strdup("/users/pandotra/tmp/data");
    // if(argc<=1){
    //     std::cout<<"Usage: $./server 1  for New Block Storage OR $./server 0  for existing block storage "<<std::endl;
    //     exit(0);
    // }
    // if (argc>1){
    //     if (argv[1][0]=='1'){
    //         long size = 256;
    //         if (argc==3){

    //             char *p;
    //             size = strtol(argv[2], &p, 10);
    //         }
    //         mountdir(mountpoint,size);
    //     }
    // }
    
    mountdir(default_mount_point,256);
    signal(SIGINT, sigintHandler);
    run_server(default_mount_point);
}
