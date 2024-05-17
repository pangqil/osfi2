#include <sys/stat.h>
#include <fcntl.h>

extern int Open(const char *path, int oflag);
extern int Creat(const char *path, mode_t mode);
extern int Read(int filedes, void *buf, size_t nbyte);
extern int Write(int filedes, const void *buf, size_t nbyte);
extern void *Mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
extern int Stat(const char *restrict path, struct stat *restrict buf);
extern int Fstat(int fildes, struct stat *buf);
extern int Shm_open(const char *name, int oflag, mode_t mode);
extern int Ftruncate(int fd, off_t length);
extern int Pthread_barrier_init(pthread_barrier_t *restrict barrier, const pthread_barrierattr_t *restrict attr, unsigned count);
extern int Pthread_barrierattr_init(pthread_barrierattr_t *attr);
extern int Pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared);
extern int Pthread_create(pthread_t *thread, const pthread_attr_t * attr, void *(*start_routine)(void *), void *arg);
