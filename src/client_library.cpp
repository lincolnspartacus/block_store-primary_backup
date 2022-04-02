#include "client_library.h"
#include "response_codes.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <sys/sendfile.h>
#include <chrono>
#include <thread>

RPCClientLibrary::RPCClientLibrary(std::vector<std::string> serverString)
{
    currentPrimary = 0; // range -> [0, 1] for two servers
    serverConnectionString = serverString;
    for (int i = 0; i < 2; i++)
        g_RPCCLient[i] = nullptr;

    for (int i = 0; i < serverString.size(); i++)
    {
        const std::string target_str = serverString[i];
        grpc::ChannelArguments ch_args;
        ch_args.SetMaxReceiveMessageSize(INT_MAX);
        ch_args.SetMaxSendMessageSize(INT_MAX);
        g_RPCCLient[i] = new BlockRPCClient(grpc::CreateCustomChannel(target_str, grpc::InsecureChannelCredentials(), ch_args));
    }
}

int RPCClientLibrary::ReadBlock(int64_t address, uint8_t *buf)
{
    bool switchServer = true;
    while (switchServer)
    {
        auto [status, reply] = g_RPCCLient[currentPrimary]->ReadBlock(address, buf);
        if (status.ok() && reply.responsecode() != RESPONSE_REDIRECT)
        {
            // Fill up the client's buffer with 4Kb data
            memcpy(buf, reply.data().c_str(), 4096);
            return 0;
        }
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
        std::cout << "\n::: Switching primary to other server :::\n"
                  << std::endl;
        currentPrimary = (currentPrimary + 1) % 2;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return -1;
}

int RPCClientLibrary::WriteBlock(int64_t address, uint8_t *buf)
{
    bool switchServer = true;
    while (switchServer)
    {
        auto [status, reply] = g_RPCCLient[currentPrimary]->WriteBlock(address, buf);
        if (status.ok() && reply.responsecode() != RESPONSE_REDIRECT)
        {
            return 0;
        }
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
        std::cout << "\n::: Switching primary to other server :::\n"
                  << std::endl;
        currentPrimary = (currentPrimary + 1) % 2;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return -1;
}
