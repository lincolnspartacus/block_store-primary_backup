#include <sys/stat.h>
#include "block_server.h"
#include "primarybackup_client.h"
#include "locks.h"
#include "state_machine.h"
#include "response_codes.h"
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <cassert>
#include <unistd.h>

extern PrimaryBackupRPCClient *g_RPCCLient; // gRPC handle to call RPCs in the other server
std::set<int64_t> BlockSet;

void printChannelState()
{
    auto state = g_RPCCLient->channel->GetState(true);
    switch (state) {
        case GRPC_CHANNEL_IDLE:
        std::cout << "CHANNEL = GRPC_CHANNEL_IDLE" << "\n";
        break;
        case GRPC_CHANNEL_CONNECTING:
        std::cout << "CHANNEL = GRPC_CHANNEL_CONNECTING" << "\n";
        break;
        case GRPC_CHANNEL_READY:
        std::cout << "CHANNEL = GRPC_CHANNEL_READY" << "\n";
        break;
        case GRPC_CHANNEL_TRANSIENT_FAILURE:
        std::cout << "CHANNEL = GRPC_CHANNEL_TRANSIENT_FAILURE" << "\n";
        break;
        case GRPC_CHANNEL_SHUTDOWN:
        std::cout << "CHANNEL = GRPC_CHANNEL_SHUTDOWN" << "\n";
        break;

        default:
        std::cout << " Channel state unknown!\n";
    }
}

void insert_block(int64_t address){
    int block_offset = address % 4096;
    BlockSet.insert(address/4096);
    if( block_offset != 0){
        BlockSet.insert((address/4096) + 1);
    } 
}

static int otherServer_IsAlive()
{
    int state_other = g_RPCCLient->GetState(StateMachine::getState());
    return state_other;
}

void local_read(int fd, uint8_t *buf, unsigned long long address){

        if(pread(fd, buf, 4096, address)==-1){
            std::cout << "Error reading block storage at offset "<<address << " " << std::endl;
            exit(EXIT_FAILURE);
        }
}

void local_write(int fd, const uint8_t *buf, unsigned long long address){

        if(pwrite(fd, buf, 4096, address)==-1){
            std::cout << "Error writing to block storage at offset "<< address << " " << std::endl;
            exit(EXIT_FAILURE);
        }
        fsync(fd);
}

BlockRPCServiceImpl::BlockRPCServiceImpl(const std::string& fileStore)
{
    mFileStore = fileStore;
    fd =  open(mFileStore.c_str(), O_RDWR, 0777);
    if(fd==-1){
        std::cout << "Error opening block storage" << std::endl;
        exit(EXIT_FAILURE);
    }
}
Status BlockRPCServiceImpl::DoMessageInt(ServerContext *context, const MessageInt *request,
                                        MessageInt *reply) {
    const int output = request->value() * 2;
    std::cout << "[server] " << request->value() << " -> " << output << " " << std::endl;
    reply->set_value(output);
    return Status::OK;
}

Status BlockRPCServiceImpl::ReadBlock(ServerContext *context, const ReadRequest *request, ReadResponse *reply)
{
    std::cout << "[ReadBlock] Requested addr = " << request->address() << "\n";

start:
    int cur_state = StateMachine::getState();
    if(cur_state == STATE_PRIMARY) {
        // TODO: Actually read from our 256gb file! @Himanshu
        
        uint8_t buf[4096];
        local_read(fd,buf,request->address());
        
        //memset(buf, 0xff, 4096);
        reply->set_data(std::string(buf, buf + 4096));
        reply->set_responsecode(RESPONSE_SUCCESS);
    } else if (cur_state == STATE_BACKUP) {
        // Check if Primary server is alive
        pthread_mutex_lock(&BACKUP_TRANSITION_LOCK);
        int other_state = otherServer_IsAlive();
        if(other_state == -1) {
            // Other server is dead. Become the primary
            StateMachine::setState(STATE_PRIMARY);
            uint8_t buf[4096];
            local_read(fd,buf,request->address());
            reply->set_data(std::string(buf, buf + 4096));
            reply->set_responsecode(RESPONSE_SUCCESS);
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
        pthread_mutex_unlock(&BACKUP_TRANSITION_LOCK);
    } else if (cur_state == STATE_START) {
        // We are booting up. Wait ...
        std::cout << "Server booting up, please wait. Retrying..\n";
        goto start;
    } else {
        std::cout << "FATAL ERROR! current state is unknown = " << cur_state;
        assert(0);
    }
    
    return Status::OK;
}

Status BlockRPCServiceImpl::WriteBlock(ServerContext *context, const WriteRequest *request, WriteResponse *reply)
{
    // Stall all writes until Resync is complete!
    std::cout << "[BlockRPCServiceImpl::WriteBlock] Trying to acquire RESYNC_LOCK\n";
    pthread_mutex_lock(&RESYNC_LOCK);
    std::cout << "[BlockRPCServiceImpl::WriteBlock] Acquired RESYNC_LOCK!\n";

    std::cout << "[BlockRPCServiceImpl::WriteBlock] Requested addr = " << request->address() << "\n";
    const uint8_t *buf = (const uint8_t *)(request->data().c_str());
    //for(int i = 0; i < 4096; ++i)
    //    printf("%x ", buf[i]);
    //printf("\n");

start:
    int cur_state = StateMachine::getState();
    std::cout << "[BlockRPCServiceImpl::WriteBlock] CurrentState = " << cur_state << "\n";

    if(cur_state == STATE_PRIMARY) {
        // TODO: Actually write to our 256gb file! @Himanshu
        std::cout << "[BlockRPCServiceImpl::WriteBlock] primary\n";

        local_write(fd,buf,request->address());
        
        // TODO: Have a global state for the other server - logging
        // Send the same request to our backup
        printChannelState();
        int ret = g_RPCCLient->WriteBlock(request);
        if(ret == -1) {
            insert_block(request->address());
            std::cout << "[WriteBlock] Backup is dead! Logging request..\n";
        }

        reply->set_responsecode(RESPONSE_SUCCESS);
    } else if (cur_state == STATE_BACKUP) {
        pthread_mutex_lock(&BACKUP_TRANSITION_LOCK);
        // Check if Primary server is alive
        int other_state = otherServer_IsAlive();
        if(other_state == -1) {
            // Other server is dead. Become the primary? TODO: RACE CONDITION!!!!!!!
            StateMachine::setState(STATE_PRIMARY);
            local_write(fd,buf,request->address());
            reply->set_responsecode(RESPONSE_SUCCESS);
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
        pthread_mutex_unlock(&BACKUP_TRANSITION_LOCK);
    } else if (cur_state == STATE_START) {
        // We are booting up. Wait ...
        std::cout << "Server booting up, please wait. Retrying..\n";
        goto start;
    } else {
        std::cout << "FATAL ERROR! current state is unknown = " << cur_state;
        assert(0);
    }
    
    pthread_mutex_unlock(&RESYNC_LOCK);
    return Status::OK;
}

const string &BlockRPCServiceImpl::getFileStorePath() const {
    return mFileStore;
}

void BlockRPCServiceImpl::setFileStorePath(const string &fileStore) {
    mFileStore = fileStore;
}
