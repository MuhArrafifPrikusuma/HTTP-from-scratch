#ifndef COMMONZIG_H
#define COMMONZIG_H

extern void *eReader(const int fd, char *restrict from_addr, void *ioptr);
extern void eWriter(const int fd, char *restrict to_addr, void *requestPtrFromC,
                    void *ioptr);

#endif // !COMMONZIG_H
