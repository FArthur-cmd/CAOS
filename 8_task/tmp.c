#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

//we can use sprintf, but it need int as argument of count of 
// positions used by number. To avoid casting to int and 
// to avoid chance of overflow this function is used
void
make_format_str(char buffer[], int64_t M, size_t number)
{
    size_t copy_of_number = number;
    for (int64_t i = M - 1; i >= 0; --i) {
        if (copy_of_number == 0) {
	    buffer[i] = ' ';
	} else {
	    buffer[i] = '0' + copy_of_number % 10;
	    copy_of_number /= 10; 
	}
    } 
}

char *
write_cycle(char *from, int64_t N, int64_t M, int64_t length, size_t *current_number)
{
    char buffer[M+1];
    buffer[M] = '\0';
    for (int64_t i = 0; i < length; ++i, ++(*current_number)) {
        make_format_str(buffer, M, *current_number); //print
        strcpy(from, buffer);	
	from[M] = ' ';
	from += M; //go to next position
    }

    from -= M; // went too far
    from += M * N + 1; //go next in raw
    --length; // one less to print in raw
    
    for (int64_t i = 0; i < length; ++i, ++(*current_number)) {
        make_format_str(buffer, M, *current_number); //print
        strcpy(from, buffer);		
	from[M] = ' ';
        from += M*N + 1;
    }

    from -= M*N + 1; //went too far
    from -= M;   // go backwards
    
    for (int64_t i = 0; i < length; ++i, ++(*current_number)) {
        make_format_str(buffer, M, *current_number); //print
        strcpy(from, buffer);		
	from[M] = ' ';

        from -= M;
    }

    from += M; //went too far
    from -= M*N + 1;//go up
    --length;
    
    for (int64_t i = 0; i < length; ++i, ++(*current_number)) {
        make_format_str(buffer, M, *current_number); //print
        strcpy(from, buffer);		
	from[M] = ' ';
        from -= M*N + 1;
    }
    from += M*N + 1; //went too far
    from += M; //prepare for next iteration;
    return from;
}

void
print_matrix(char *start, int64_t N, int64_t M)
{
    size_t current_number = 1;
    size_t number_of_iterations = N/2 + N%2;
    char *current = start;
    for (int64_t i = 0; i < number_of_iterations; ++i) {
       current = write_cycle(current, N, M, N - 2*i, &current_number);
    }
    for (int64_t i = 0; i < N - 1; ++i) {
        start[(i + 1)*N*M + i] = '\n';
    }
}

int
main(int argc, char *argv[])
{
   char *file = argv[1];
   int64_t N = strtol(argv[2], NULL, 10);
   int64_t M = strtol(argv[3], NULL, 10);
   int fd = open(file, O_RDWR | O_CREAT, 0666);
   ftruncate(fd, N*(N*M + 1));
   char *contents = mmap(NULL,
		         N*N*M + N,
			 PROT_READ | PROT_WRITE,
			 MAP_SHARED,
			 fd,
			 0);
   memset(contents, ' ', N*M*M+N - 1); 
   print_matrix(contents, N, M);
   munmap(contents, N*N*M + N);
   close(fd);
   return 0;
}
