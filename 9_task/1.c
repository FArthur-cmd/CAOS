#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum {
    INPUT_SIZE = 4096,
    OUTPUT_SIZE = 5003 //input_size + 'print()'
} constants;

int
main()
{
    char input[INPUT_SIZE];
    char output[OUTPUT_SIZE];
    
    if (NULL == fgets(input, sizeof(input), stdin)) {      //read str
        perror(NULL);
	exit(1);
    }

    //make str right (make \0 at the end)

    char *end_of_line = memchr(input, '\n', sizeof(input));
    if (NULL != end_of_line) {
        *end_of_line = '\0';
    }


    //create print(input) to print calculations

    strcpy(output, "print(\0");
    strcat(output, input);
    strcat(output, ")\0");

    //call python3 (find it in PATH settings and get flag -c 
    //to calculate without starting

    if (-1 == execlp("python3", "python3", "-c", output, NULL)) {
        perror(NULL);
	exit(1);
    }

    exit(0);
}
