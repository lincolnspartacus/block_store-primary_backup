#include <string.h>
#include <errno.h>
#include <iostream>
#include "client_library.h"
#include <sys/stat.h>

RPCClientLibrary *gRPCClient = nullptr;

int compare_buf(uint8_t *buf1, uint8_t *buf2, int len)
{
    for(int i = 0; i < len; ++i)
      if(buf1[i] != buf2[i])
        return 1;

    return 0;
}

void write_random_overlapping()
{
    // Writes to B0, B1, B2, B3, 3 Overlapping addresses
    int offsets[] = {0, 4096*1, 4096*2, 4096*3, 1024, 3084,  4096*3 - 2345};

    int blocks = sizeof(offsets)/sizeof(int);
    // Perform same operations on this in-memory chunk, for comparison
    uint8_t *in_mem_file = new uint8_t[4096 * 4];

    for(int i = 0; i < blocks; ++i) {
        uint8_t block_i[4096];
        for(int j = 0; j < 4096; ++j)
          block_i[j] = (uint8_t)rand();

        int answer = gRPCClient->WriteBlock(offsets[i], block_i);
        printf("Write Ret for Block %d = %d\n", i, answer);

        memcpy(in_mem_file + offsets[i], block_i, 4096);
    }

    uint8_t *recvd_file = new uint8_t[4096 * 4];
    for(int i = 0; i < blocks; ++i) {
        int answer = gRPCClient->ReadBlock(offsets[i], recvd_file + offsets[i]);
    }

    if(compare_buf(recvd_file, in_mem_file, 4096 * 4) == 0) {
        printf("write_random_overlapping() : Test PASSED!\n");
    } else {
        printf("write_random_overlapping() : Test FAILED :(\n");
    }
}

void write_random_non_overlapping()
{
    srand(500);
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

    // char user_input[10];
    // printf("Go crash the server now!\n");
    // scanf("%s", user_input);

    // Read back the same offsets and compare it
    uint8_t rcv_buf[4096];
    for(int i = 0; i < blocks; ++i) {
      int answer = gRPCClient->ReadBlock(offsets[i], rcv_buf);
      if(compare_buf(rcv_buf, buf[i], 4096) == 1) {
          printf("write_random_non_overlapping() : Test Test PASSED!\n");
          return;
      }
    }

    printf("write_random_non_overlapping() : Test FAILED :(\n");
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
  
  //write_random_non_overlapping();
  write_random_overlapping();

  return 0;
}
