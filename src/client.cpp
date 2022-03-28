#include <string.h>
#include <errno.h>
#include <iostream>
#include "block_client.h"
#include <sys/stat.h>

BlockRPCClient* g_RPCCLient = nullptr;


int main(int argc, char *argv[])
{
  
  const std::string target_str = "localhost:50051";
  grpc::ChannelArguments ch_args;
  ch_args.SetMaxReceiveMessageSize(INT_MAX);
  ch_args.SetMaxSendMessageSize(INT_MAX);
  g_RPCCLient =  new BlockRPCClient ( grpc::CreateCustomChannel(target_str, grpc::InsecureChannelCredentials() , ch_args ) ); 
  int sent = 10;
  int answer = g_RPCCLient->DoMessageInt(sent);
  printf("Sent is %d\n",sent);
  printf("Reply is %d\n",answer);
  return 0;
}
