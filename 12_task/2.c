#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int
main(int argc, char *argv[])
{
    in_port_t port = htons(strtol(argv[1], NULL, 10));

    //fill sockaddr
    struct sockaddr_in full_addr;
    full_addr.sin_family = AF_INET;
    full_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    full_addr.sin_port = port;


    //create udp socket
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock_fd == -1) {
       perror(NULL);
       exit(0);

    }

    int number_send, number_receive;

    while (scanf("%d", &number_send) > 0) {
        sendto(sock_fd,
	       &number_send,
	       sizeof(number_send),
	       0,
	       (const struct sockaddr*)&full_addr,
	       sizeof(full_addr));
	recvfrom(sock_fd,
		 &number_receive,
		 sizeof(number_receive),
		 0,
		 NULL,
		 NULL);
	printf("%d\n", number_receive);
    }

    close(sock_fd);

    return 0;
}
