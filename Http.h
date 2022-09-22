
#ifndef HTTP
#define HTTP

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/ssl.h>

#define BUFFER 1600000
#define PORT 443
#define HTTPBUFFER 1000

typedef struct httpHeader {
	char *host;
	char *url;
	struct httpHeader *next;
} header;

void httpProduct(header *head);

#endif // HTTP

