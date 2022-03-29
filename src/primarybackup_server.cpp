#include <sys/stat.h>
#include "primarybackup_server.h"
#include "state_machine.h"
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>

Status PrimaryBackupRPCServiceImpl::GetState(ServerContext *context, const MessageInt *request,
                                        MessageInt *reply) {
    std::cout << "[server] Inside GetState()" << "\n";
    reply->set_value(StateMachine::getState());
    return Status::OK;
}
