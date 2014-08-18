#ifndef JAIL_MMAP_HPP
#define JAIL_MMAP_HPP

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

struct mmap_t {
  mmap_t()
    :fd(-1)
    ,data(NULL) {}
 
  // return 0 on success
  int load(const char *fname) {
    fd = open(fname, O_RDONLY);
    if (fd == -1) {
      perror("Error opening file for reading");
      return -1;    
    }

    struct stat st;
    fstat(fd, &st);
    size = st.st_size;

    data = mmap(0, size, PROT_READ, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
      close(fd);
      perror("Error mmapping the file");
      return -1;
    }
    
    return 0;  
  }
  
  // returns 0 on success
  int free() {
    if (munmap(data, size) == -1) {
      perror("Error un-mmapping the file");
      return -1;
    }
    close(fd);
    return 0;
  }
  int fd;
  void *data;
  size_t size;
};

#endif
