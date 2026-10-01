#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
extern char _heap_start, _heap_end;

void *_sbrk(ptrdiff_t increment) {
    static char *cursor = &_heap_start;
    if (increment < 0 || (uintptr_t)increment > (uintptr_t)(&_heap_end - cursor)) {
        errno = ENOMEM;
        return (void *)-1;
    }
    char *previous = cursor;
    cursor += increment;
    return previous;
}
int _write(int fd, char *data, int length) {
    (void)fd;
    for (int i = 0; i < length; i++)
        fputc((unsigned char)data[i], stdout);
    return length;
}
int _read(int fd, char *data, int length) {
    (void)fd;
    (void)data;
    (void)length;
    errno = ENOSYS;
    return -1;
}
int _close(int fd) {
    (void)fd;
    return -1;
}
int _fstat(int fd, struct stat *s) {
    (void)fd;
    s->st_mode = S_IFCHR;
    return 0;
}
int _isatty(int fd) {
    (void)fd;
    return 1;
}
off_t _lseek(int fd, off_t offset, int whence) {
    (void)fd;
    (void)offset;
    (void)whence;
    return 0;
}
int _getpid(void) { return 1; }
int _kill(int pid, int sig) {
    (void)pid;
    (void)sig;
    errno = EINVAL;
    return -1;
}
void _exit(int status) {
    (void)status;
    for (;;) {
    }
}
