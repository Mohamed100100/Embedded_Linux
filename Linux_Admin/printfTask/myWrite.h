#ifndef MY_WRITE_H_
#define MY_WRITE_H_

ssize_t my_write(int fd, const void *buf, ssize_t count);
ssize_t myPrintf(const void *buf);

#endif
