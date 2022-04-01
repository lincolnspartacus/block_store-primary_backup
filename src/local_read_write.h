#include <stdint.h>
void local_read(int fd, uint8_t *buf, unsigned long long address);
void local_write(int fd, const uint8_t *buf, unsigned long long address);
