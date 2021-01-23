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
#include <sys/epoll.h>
#include <ctype.h>

typedef enum {
   BUF_SIZE = 4096
} constants;

#define GOT_ERROR(number) (-1==(number))

volatile sig_atomic_t must_exit = 0;

void
handler(int signum)
{
    must_exit = 1;
}

static inline void
set_ending_signals(struct sigaction *act)
{
    memset(act, 0, sizeof(*act));
    act->sa_handler = handler;
    act->sa_flags = SA_RESTART;
    sigaction(SIGTERM, act, NULL);
}

static void
make_non_blocking(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

//use fd from union data
void
add_in_epoll(int ep_fd, int fd)
{
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;
    epoll_ctl(ep_fd, EPOLL_CTL_ADD, fd, &event);
}

// if updates on server, then new fds have to be added
void
process_server(int epoll_fd, int sock_fd)
{
    int connected_fd = accept(sock_fd, NULL, NULL);
    if (!GOT_ERROR(connected_fd)) {
	make_non_blocking(connected_fd);
	add_in_epoll(epoll_fd, connected_fd);
    }
}

//if client update, read update and make transformation
void
process_client(int client_fd)
{
    char buffer[BUF_SIZE];
    ssize_t read_count = read(client_fd, buffer, sizeof(buffer));
    if(!GOT_ERROR(read_count)) {
        for (ssize_t i = 0; i < read_count; ++i) {
	    
	    //make all symbols in upper case
	    
	    buffer[i] = toupper(buffer[i]);
	}

	write(client_fd, buffer, read_count);
    } else {

	shutdown(client_fd, SHUT_RDWR);
        close(client_fd);
    }
}

//check what type of process is now
void
work_on_process(int epoll_fd, struct epoll_event *event, int sock_fd)
{
    if (event->data.fd == sock_fd) {
        process_server(epoll_fd, sock_fd);
    } else {
        process_client(event->data.fd);
    }
}

//first part is from 12_contest (typical server)
//second part has a lot in common with 0.c from 13_contest
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
    
    //create socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (GOT_ERROR(sock_fd)) {
       perror("creating socket failed");
       exit(1);
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
        exit(1);
    }

    //make queue of connections
    //SOMAXCONN - max count of connections
    if (GOT_ERROR(listen(sock_fd, SOMAXCONN))) {
        perror("listen failed");
        exit(1);
    }

    //we don't know count of fds
    int epoll_fd = epoll_create1(0); 
    if (GOT_ERROR(epoll_fd)) {
        perror(NULL);
	exit(1);
    }
    //add server in epoll
    add_in_epoll(epoll_fd, sock_fd);
    
    //as max number of clients in queue is SOMAXCONN
    //this is max number of events(update in client or server)
    struct epoll_event pending[SOMAXCONN];
    while (!must_exit) {
        ssize_t count = epoll_wait(epoll_fd, pending, SOMAXCONN, -1);
	for (ssize_t i = 0; i < count; ++i) {
	    work_on_process(epoll_fd, &pending[i], sock_fd);
	}
    }
    close(epoll_fd);
    close(sock_fd);

    return 0;
}
