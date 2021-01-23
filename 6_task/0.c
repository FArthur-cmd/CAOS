#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int
main(int argc, char *argv[])
{
    int exit_code = 0;
    int output_first = -1;
    int output_second = -1;
    int input = open(argv[1], O_RDONLY);
    if (input == -1) {
        exit_code = 1;
	goto end;
    }
    output_first = open(argv[2], O_WRONLY | O_CREAT, 0640);
    output_second = open(argv[3], O_WRONLY | O_CREAT, 0640);
    if (output_first == -1 || output_second == -1) {
        exit_code = 2;
	goto end;
    }
    ssize_t input_status;
    char symbol;
    int output;
    
    while ((input_status = read(input, &symbol, sizeof(symbol))) > 0 ) {
       output = (symbol <= '9' && symbol >='0') ? output_first : output_second;
       if (write(output, &symbol, sizeof(symbol)) == -1) {
           exit_code = 3;
           goto end;	   
       }
    }

    if (input_status == -1) {
        exit_code = 3;
    }
end:
    if (input != -1) {
        close(input);
    }
    if (output_first != -1) {
        close(output_first);
    }
    if (output_second != -1) {
        close(output_second);
    }
    return exit_code;
}
