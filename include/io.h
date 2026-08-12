#ifndef IO_H
#define IO_H
#include "../server/server.h"

void Writer(int32_t fd);
void Reader(int32_t fd, size_t max_size);

#endif // !IO_H
