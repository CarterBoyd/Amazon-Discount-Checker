/**
  * this is the mailing part of the program where once it has the information it will send the data
  * to an email
  */

#define SMTPPORT 25

#include <sys/socket.h>
#include <strings.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
/*
int main() {
	int socketfd = socket(PF_INET, SOCK_STREAM, 0);
	if (socketfd == -1) {
		perror("socket");
		return 1;
	}


	struct hostent *hostent = gethostbyname("smtp.gmail.com");
	struct sockaddr_in sockaddr;
	if (hostent == NULL) {
		perror("hostent");
		return 1;
	}
	bzero(&sockaddr, sizeof(sockaddr));
	const char *stringAddr = inet_ntoa(*(struct in_addr*) *(hostent->h_addr_list));
	sockaddr.sin_addr.s_addr = inet_addr(stringAddr);
	sockaddr.sin_family = PF_INET;
	sockaddr.sin_port = htons(SMTPPORT);

	if (connect(socketfd, (struct sockaddr*) &sockaddr, sizeof(sockaddr))) {
		perror("connect");
		return 1;
	}
	printf("sending helo: ");
	send(socketfd, "HELO", 4, 0);
	printf("sent\n");
	char buff[5];
	printf("checking to receive: ");
	recv(socketfd, buff, 4, 0);
	printf("received\n");
	printf("%5s\n", buff);
	return 0;
}
*/
