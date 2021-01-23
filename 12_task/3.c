#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

typedef enum {
    MAX_SIZE = 4096,
    DONT_EXIST = 1,
    CANT_READ = 2,
    ALL_OK = 3,
    ENDING_SYMBOLS_COUNT = 2,
    CHILD = 0
} constants;

#define GOT_ERROR(number) (-1==number)

volatile sig_atomic_t must_exit = 0;
volatile sig_atomic_t has_connection = 0;
volatile int32_t sock_fd = -1;

// if have connection, complete it
// else end programm
void
handler(int signum)
{
    must_exit = 1;
    if (has_connection == 0) {
      if (sock_fd > 0) {
          close(sock_fd);
	  _exit(0);
      }
    }
}

static inline void
set_ending_signals(struct sigaction *act)
{
    memset(act, 0, sizeof(*act));
    act->sa_handler = handler;
    act->sa_flags = SA_RESTART;
    sigaction(SIGTERM, act, NULL);
    sigaction(SIGINT, act, NULL);
}

// find end of line as it must be in input
// if previous sybol is \n there was empty line
void
wait_end_of_input(int in_fd, char *input)
{
    char *current;
    do { 
        current = input;
        while (NULL != (current=strstr(current, "\r\n"))) {
	    if (*(current - 1) == '\n') 
        	return;
	    current += ENDING_SYMBOLS_COUNT;
        }
    } while (read(in_fd, input, MAX_SIZE) > 0);
}

//check can we find file
//then can we read it
int16_t
file_status(char *file) 
{
   if (access(file, F_OK) == -1) {
       return DONT_EXIST;
   }
   
   if (access(file, R_OK) == -1) {
       return CANT_READ;
   }
  
   return ALL_OK;
}

//if we can send, put all data as it asked to put
void
send_data(int fd, char *file)
{
    char *success = "HTTP/1.1 200 OK\r\n\0";
    char content[MAX_SIZE];
    char buffer[MAX_SIZE];

    struct stat info;
    stat(file, &info);

    uint32_t already_read = 0;
    uint32_t left_to_read = info.st_size;

    sprintf(content, "Content-Length: %lu\r\n\r\n", info.st_size);
    
    write(fd, success, strlen(success)*sizeof(char));
    write(fd, content, strlen(content)*sizeof(char));

    int file_fd = open(file, O_RDONLY);
    while ((already_read = read(file_fd, buffer, left_to_read)) > 0) {
        write(fd, buffer, already_read);
	left_to_read -= already_read;
    }
 
    close(file_fd);
}

void
send_forbidden(int fd)
{
    char *forbidden = "HTTP/1.1 403 Forbidden\r\n\0";
    char *empty_content = "Content-Length: 0\r\n\r\n\0";
    write(fd, forbidden, strlen(forbidden)*sizeof(char));
    write(fd, empty_content, strlen(empty_content)*sizeof(char));
}

void
send_not_found(int fd)
{
    char *not_found = "HTTP/1.1 404 Not Found\r\n\0";
    char *empty_content = "Content-Length: 0\r\n\r\n\0";
    write(fd, not_found, strlen(not_found)*sizeof(char));
    write(fd, empty_content, strlen(empty_content)*sizeof(char));
}

//create new process
//make writing in fd = output
//exec program
void
execute(int fd, char *name)
{
    char *success = "HTTP/1.1 200 OK\r\n\r\n\0";
  
    write(fd, success, strlen(success)*sizeof(char));

    pid_t pid = fork();
    
    if (pid == CHILD) {
        dup2(fd, 1);
	close(fd);
	execl(name, name, NULL);
	perror(NULL);
    } else {
        waitpid(pid, 0, 0);
    }
}

//read file_name from first line
//create full path
//wait empty line from input
//check file and print answer for task
void
work_with(int in_fd, const char* path)
{
    char name[MAX_SIZE];
    char input[MAX_SIZE];
    memset(name, 0, sizeof(name));
    memset(input, 0, sizeof(input));


    read(in_fd, input, MAX_SIZE);
    sscanf(input, "GET %s HTTP/1.1", name);

    wait_end_of_input(in_fd, input);
    
    strcpy(input, path);
    strcat(input, "/");
    strcat(input, name);

    int16_t status = file_status(input);

    if (status == ALL_OK) {
	//check if executable
	if (access(input, X_OK) != -1) {
             execute(in_fd, input); 
	} else {
             send_data(in_fd, input);
	}
    } else if (status == DONT_EXIST) {
        send_not_found(in_fd);
    } else if (status == CANT_READ) {
        send_forbidden(in_fd);
    }
}

int
main(int argc, char *argv[])
{
    //make signals handler
    struct sigaction ending_program;

    set_ending_signals(&ending_program);

    //get ip and port
    //127.0.0.1 - localhost
    in_port_t port = htons(strtol(argv[1], NULL, 10));
    in_addr_t addr = inet_addr("127.0.0.1");
    const char *path_to_data = argv[2];

    //create socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (GOT_ERROR(sock_fd)) {
       perror("creating socket failed");
       exit(-1);
    }

    //create IPv4 struct for binding server
    struct sockaddr_in info;
    info.sin_family = AF_INET;
    info.sin_port = port;
    info.sin_addr.s_addr = addr;

    //bind
    if (GOT_ERROR(bind(sock_fd,
		       (const struct sockaddr*)&info, 
		       sizeof(info)))) {
        perror("bind failed");
	exit(-1);
    }

    //make queue of connections
    //SOMAXCONN - max count of connections
    if (GOT_ERROR(listen(sock_fd, SOMAXCONN))) {
        perror("listen failed");
	exit(-1);
    }
    
    int connected_fd;
    while (!must_exit) {
	// take one connection and work with it
        connected_fd = accept(sock_fd, NULL, NULL);

	if (GOT_ERROR(connected_fd)) {
	    perror("wrong connection");
	    exit(1);
	}

	has_connection = 1;
        work_with(connected_fd, path_to_data);
	shutdown(connected_fd, SHUT_RDWR);
	close(connected_fd);
	has_connection = 0;
    }

    if (sock_fd > 0) {
        close(sock_fd);
    }
    return 0;
}
