#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

struct Item {
    int value;
    uint32_t next_pointer;
};

int
main(int argc, char *argv[])
{
    char *file = argv[1];
    int32_t fd = open(file, O_RDONLY);
    struct stat info;
    fstat(fd, &info);
    if (info.st_size == 0) {
	close(fd);
	return 0;
    }
    char *contents = mmap(NULL,
		          info.st_size,
			  PROT_READ,
			  MAP_PRIVATE,
			  fd,
			  0);
    struct Item got_from_file = *(struct Item*)contents;
    while (got_from_file.next_pointer != 0) {
        printf("%d ", got_from_file.value);
        got_from_file = *(struct Item*)(contents + got_from_file.next_pointer);
    }
    printf("%d ", got_from_file.value);
    munmap(contents, info.st_size);
    close(fd);
    return 0;
}
