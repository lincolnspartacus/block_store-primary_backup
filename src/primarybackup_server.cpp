#include <sys/stat.h>
#include "primarybackup_server.h"
#include "primarybackup_client.h"
#include "locks.h"
#include "state_machine.h"
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>

extern std::set<int64_t> BlockSet;
extern PrimaryBackupRPCClient *g_RPCCLient; // gRPC handle to call RPCs in the other server

Status PrimaryBackupRPCServiceImpl::GetState(ServerContext *context, const MessageInt *request,
                                        MessageInt *reply) {
    std::cout << "[server] Inside GetState()" << "\n";
    reply->set_value(StateMachine::getState());
    return Status::OK;
}

/*
 * Executed by the Backup as part of the replication process
 */
Status PrimaryBackupRPCServiceImpl::WriteBlock(ServerContext *context, const WriteRequest *request, WriteResponse *reply)
{
    // TODO: Actually write to our 256Gb file. @Himanshu
    std::cout << "[PrimaryBackupRPCServiceImpl::WriteBlock] Inside\n";
    std::cout << "[PrimaryBackupRPCServiceImpl::WriteBlock] Requested addr = " << request->address() << "\n";
    const uint8_t *buf = (const uint8_t *)(request->data().c_str());
    for(int i = 0; i < 4096; ++i)
        printf("%x ", buf[i]);
    printf("\n");


    return Status::OK;
}

Status PrimaryBackupRPCServiceImpl::ReSync(ServerContext* context, const Empty *empty_req, ServerWriter<WriteRequest> *writer) {
    
    pthread_mutex_lock(&RESYNC_LOCK);

    std::cout << "[PrimaryBackupRPCServiceImpl::ReSync] Inside here "<<std::endl;
    WriteRequest request;

    for (auto it=BlockSet.begin(); it!=BlockSet.end(); ++it){
        request.clear_data();
        request.set_address(*it * 4096);
        char buf[4096];
        memset(buf,0xff,4096);
        request.set_data(std::string(buf,buf+4096));
        writer->Write(request);
    }

    std::cout << "[PrimaryBackupRPCServiceImpl::ReSync] Primary side resync done" << endl;

    pthread_mutex_unlock(&RESYNC_LOCK);
    return Status::OK;
}