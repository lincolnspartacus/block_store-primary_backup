#include "block_client.h"

class RPCClientLibrary
{

public:
    RPCClientLibrary(std::vector<std::string> serverConnectionString);

    int ReadBlock(int64_t address, uint8_t *buf);
    int WriteBlock(int64_t address, uint8_t *buf);

private:
    BlockRPCClient *g_RPCCLient[2];
    std::vector<std::string> serverConnectionString;
    int currentPrimary;
};