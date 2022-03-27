#include "block_server.h"
#include <fcntl.h>
#include <sys/stat.h>

void sigintHandler(int sig_num)
{
    std::cerr << "Clean Shutdown\n";
    //    if (srv_ptr) {
    //        delete srv_ptr;
    //    }
    fflush(stdout);
    std::exit(0);
}

void mountdir(char* root){

    int res = mkdir(root, 0777);
    //int res = mkdir(root, S_IRWXU | S_IRWXG | S_IRWXO);
    printf("Filesystem mounted on %s\n",root);
}

void run_server(char* loc)
{
    std::string mountpoint(loc);
    std::string server_address("localhost:50051");
    FileRPCServiceImpl service(mountpoint);
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
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char* argv[])
{
    char* mountpoint;
    if (argc>1){
        mountpoint = argv[1];
    }
    else{
    	mountpoint = strdup("/users/pandotra/fs/serverData");
    }

    mountdir(mountpoint);
    signal(SIGINT, sigintHandler);
    run_server(mountpoint);
}
