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

  uint8_t *buf = new uint8_t[4096];
  // int answer = g_RPCCLient->ReadBlock(56, buf);

  // printf("Ret = %d\n", answer);
  // for(int i = 0; i < 4096; ++i)
  //   printf("%x ", buf[i]);

  memset(buf, 0xff, 4096);
  int answer = g_RPCCLient->WriteBlock(67, buf);
  printf("Ret = %d\n", answer);

  return 0;
}
