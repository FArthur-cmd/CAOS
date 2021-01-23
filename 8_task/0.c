#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
    char *file = argv[1];
    char *pattern = argv[2];
    int fd = open(file, O_RDONLY);
    struct stat info;
    fstat(fd, &info);
    if (info.st_size == 0) {
        close(fd);
        exit(0);
    }
    char *contents = mmap(NULL,
                          info.st_size,
                          PROT_READ,
                          MAP_PRIVATE,
                          fd,
                          0);
    char *entry = contents;    
    while (NULL!=(entry=strstr(entry, pattern))) {
        size_t diff = entry - contents;
        printf("%lu ", diff);
        ++entry;
    }
    printf("\n");
    munmap(contents, info.st_size);
    close(fd);
    return 0;
}
