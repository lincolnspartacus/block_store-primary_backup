#include <string.h>
#include <errno.h>
#include <iostream>
#include "client_library.h"
#include <sys/stat.h>

RPCClientLibrary *gRPCClient = nullptr;

int main(int argc, char *argv[])
{

  std::vector<std::string> serverString;
  printf("*** Using new client ****\n");
  serverString.push_back("c220g1-031114.wisc.cloudlab.us:50051"); // Node 0
  serverString.push_back("c220g1-031111.wisc.cloudlab.us:50051"); // Node 1

  RPCClientLibrary *gRPCClient = new RPCClientLibrary(serverString);

  // int sent = 10;
  // int answer = gRPCClient->DoMessageInt(sent);

  // printf("Sent is %d\n", sent);
  // printf("Reply is %d\n", answer);

  printf("Testing read & write: \n");
  uint8_t *buf = new uint8_t[4096];
  // answer = gRPCClient->ReadBlock(56, buf);
  // printf("Read Ret = %d\n", answer);
  // for(int i = 0; i < 4096; ++i)
  //   printf("%x ", buf[i]);

  memset(buf, 66, 4096);
  int answer = gRPCClient->WriteBlock(0, buf);
  printf("Write Ret = %d\n", answer);

  return 0;
}
