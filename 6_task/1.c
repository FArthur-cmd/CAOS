#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdio.h>


struct Item {
  int value;
  uint32_t next_pointer;
};

int
main(int argc, char *argv[])
{
    int input_file = open(argv[1], O_RDONLY);
    if (input_file == -1) {
        goto end;
    }
    
    struct Item input_item;
    ssize_t input_status;

    while ((input_status = read(input_file, &input_item, sizeof(input_item))) > 0) {
        printf("%d ", input_item.value);
        if (input_item.next_pointer > 0) {
	    if (lseek(input_file, input_item.next_pointer, SEEK_SET) == -1) {
	        goto end;
	    } 
	} else {
	    break;
	}	
    }

end:
    close(input_file);
    return 0;
}
