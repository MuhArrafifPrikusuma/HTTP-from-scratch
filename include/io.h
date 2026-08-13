#ifndef IO_H
#define IO_H

#include "common.h"

// extern void Writer(int32_t fd);
extern void *Reader(int32_t fd, char *from_addr);

#endif // !IO_H
