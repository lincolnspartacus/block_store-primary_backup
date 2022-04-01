#include "local_read_write.h"
#include <iostream>
#include <unistd.h>

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