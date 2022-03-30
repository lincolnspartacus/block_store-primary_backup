#include <sys/stat.h>
#include "block_server.h"
#include "primarybackup_client.h"
#include "state_machine.h"
#include "response_codes.h"
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <cassert>

extern PrimaryBackupRPCClient *g_RPCCLient; // gRPC handle to call RPCs in the other server

static int otherServer_IsAlive()
{
    int state_other = g_RPCCLient->GetState(5);
    return state_other;
}

BlockRPCServiceImpl::BlockRPCServiceImpl(const std::string& fileStore)
{
    mFileStore = fileStore;
}
Status BlockRPCServiceImpl::DoMessageInt(ServerContext *context, const MessageInt *request,
                                        MessageInt *reply) {
    const int output = request->value() * 2;
    std::cout << "[server] " << request->value() << " -> " << output << "\n";
    reply->set_value(output);
    return Status::OK;
}

Status BlockRPCServiceImpl::ReadBlock(ServerContext *context, const ReadRequest *request, ReadResponse *reply)
{
    std::cout << "[ReadBlock] Requested addr = " << request->address() << "\n";

    int cur_state = StateMachine::getState();
    if(cur_state == STATE_PRIMARY) {
        // TODO: Actually read from our 256gb file! @Himanshu
        uint8_t buf[4096];
        memset(buf, 0xff, 4096);
        reply->set_data(std::string(buf, buf + 4096));
        reply->set_responsecode(RESPONSE_SUCCESS);
    } else if (cur_state == STATE_BACKUP) {
        // Check if Primary server is alive
        int other_state = otherServer_IsAlive();
        if(other_state == -1) {
            // Other server is dead. Become the primary? TODO: RACE CONDITION!!!!!!!
        } else if(other_state == STATE_PRIMARY) {
            // Redirect
            reply->set_responsecode(RESPONSE_REDIRECT);
        } else if (other_state == STATE_BACKUP) {
            std::cout << "FATAL ERROR! Both servers are in STATE_BACKUP";
            assert(0);
        } else if (other_state == STATE_START) {
            // Other server is booting up.. Wait for it and then redirect
            reply->set_responsecode(RESPONSE_REDIRECT);
        } else {
            std::cout << "FATAL ERROR! Other state is unknown = " << other_state;
            assert(0);
        }
    } else if (cur_state == STATE_START) {
        // We are booting up. Wait ...
    } else {
        std::cout << "FATAL ERROR! current state is unknown = " << cur_state;
        assert(0);
    }
    
    return Status::OK;
}

Status BlockRPCServiceImpl::WriteBlock(ServerContext *context, const WriteRequest *request, WriteResponse *reply)
{
    return Status::OK;
}

const string &BlockRPCServiceImpl::getFileStorePath() const {
    return mFileStore;
}

void BlockRPCServiceImpl::setFileStorePath(const string &fileStore) {
    mFileStore = fileStore;
}
