#include <sys/stat.h>
#include "primarybackup_server.h"
#include "primarybackup_client.h"
#include "state_machine.h"
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>

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