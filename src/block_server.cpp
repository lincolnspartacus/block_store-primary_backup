#include <sys/stat.h>
#include "block_server.h"
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>


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

const string &BlockRPCServiceImpl::getFileStorePath() const {
    return mFileStore;
}

void BlockRPCServiceImpl::setFileStorePath(const string &fileStore) {
    mFileStore = fileStore;
}
