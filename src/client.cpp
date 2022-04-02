#include <string.h>
#include <errno.h>
#include <iostream>
#include "client_library.h"
#include <sys/stat.h>

RPCClientLibrary *gRPCClient = nullptr;

int compare_buf(uint8_t *buf1, uint8_t *buf2)
{
    for(int i = 0; i < 4096; ++i)
      if(buf1[i] != buf2[i])
        return 1;

    return 0;
}

void write_random_non_overlapping()
{
    constexpr int blocks = 4;
    int offsets[blocks] = {0, 4096, 4096*2 + 234, 4096*10 + 76};
    uint8_t *buf[blocks];

    for(int i = 0; i < blocks; ++i)
      buf[i] = new uint8_t[4096];

    for(int i = 0; i < blocks; ++i) {
      // Fill up buf with random data
      for(int j = 0; j < 4096; ++j)
        buf[i][j] = (uint8_t)rand();
      
      int answer = gRPCClient->WriteBlock(offsets[i], buf[i]);
      printf("Write Ret for Block %d = %d\n", i, answer);
    }

    char user_input[10];
    printf("Go crash the server now!\n");
    scanf("%s", user_input);

    // Read back the same offsets and compare it
    uint8_t rcv_buf[4096];
    for(int i = 0; i < blocks; ++i) {
      int answer = gRPCClient->ReadBlock(offsets[i], rcv_buf);
      if(compare_buf(rcv_buf, buf[i]) == 1) {
          printf("write_random_non_overlapping() : Test failed!\n");
          return;
      }
    }

    printf("write_random_non_overlapping() : Test passed!\n");
}

int main(int argc, char *argv[])
{

  std::vector<std::string> serverString;
  printf("*** Using new client ****\n");
  serverString.push_back("c220g1-031114.wisc.cloudlab.us:50051"); // Node 0
  serverString.push_back("c220g1-031111.wisc.cloudlab.us:50051"); // Node 1

  gRPCClient = new RPCClientLibrary(serverString);

  uint8_t *buf = new uint8_t[4096];

  // while(1){
  //   memset(buf, 66, 4096);
  //   int answer = gRPCClient->WriteBlock(0, buf);
  //   printf("Write Ret = %d\n", answer);

  //   char user_input[10];
  //   scanf("%s", user_input);
  // }
  write_random_non_overlapping();

  return 0;
}
