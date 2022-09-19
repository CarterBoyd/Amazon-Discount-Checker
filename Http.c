#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/ssl.h>

#define BUFFER 160000
#define PORT 443
#define HTTPBUFFER 1000


/**
  * when a program has failed it will always send out to perror then exit with a failure
  */
static inline void error(const char *error) {
	perror(error);
	exit(EXIT_FAILURE);
}

/**
  * If the old buffer is close to capacity it will create a new buffer
  * that is twice the size and copy all the information from the old to
  * the new, freeing the old buffer and returning the new one
  */
static inline char *resizeBuffer(char *oldBuffer, const int oldSize) {
	char *newBuffer = malloc(sizeof(char) * (oldSize * 2));
	memcpy(newBuffer, oldBuffer, oldSize);
	free(oldBuffer);
	return newBuffer;
}

/**
  * creates the socket and checks for errors created by the socket
  */
static const int createSocket() {
	struct protoent *protoent = getprotobyname("tcp");
	if (protoent == NULL)
		error("getprotobyname");
	const int soc = socket(PF_INET, SOCK_STREAM, protoent->p_proto);
	if (soc == -1)
		error("socket");
	return soc;
}

/**
  * mallocs the format of the request
  * const char url, the path in the website that will be httped into
  * const char host, the host of the website
  * returns: the http request that will be sent out
  */
static char *createHTTPRequest(const char *url, const char *host) {
	const char msg[] = "GET %s HTTP/1.1\r\n"
			"Host: %s\r\n"
			"Connection: close\r\n"
			"\r\n";
	char output[HTTPBUFFER];
	int length = sprintf(output, msg, url, host);
	char *completeMsg = malloc(length * sizeof(char));
	memcpy(completeMsg, output, length);
	completeMsg[length + 1] = '\0';
	return completeMsg;
}

/**
  * Creates the socket address and fills out information for the address
  * host, the name of the host that will be httped into
  * returns: the socket address that will have all the http server information filled
  */
static struct sockaddr_in connectToAddress(const char *host) {
	struct hostent *hostent = gethostbyname(host);
	struct sockaddr_in sockaddr;
	if (hostent == NULL)
		error("hostent");
	bzero(&sockaddr, sizeof(sockaddr));
	const char *stringAddress = inet_ntoa(*(struct in_addr*)*(hostent->h_addr_list));
	sockaddr.sin_addr.s_addr = inet_addr(stringAddress);	
	sockaddr.sin_family = PF_INET;
	sockaddr.sin_port = htons(PORT);
	return sockaddr;
}

/**
  * connects to the server and checks for errors
  */
static inline void connectToServer(const int socketfd, const struct sockaddr_in sockaddrin) {
    if (connect(socketfd, (struct sockaddr*) &sockaddrin, sizeof(sockaddrin)))
        error("connect");
}

/**
  * takes the message and sends it to the http host
  * const int socketfd, the socket that is conencted to the host
  * const char msg, the http message
  */
static void sendMsg(SSL *socketfd, const char *msg) {
	ssize_t bytesTotal = 0, bytesSent = 0;
	size_t msgLen = strlen(msg);
	printf("Sending:\n%s\n", msg);
	while (bytesTotal < msgLen) {
		bytesSent = SSL_write(socketfd, msg + bytesTotal, msgLen - bytesTotal);
		printf("%zd bytes sent\n", bytesSent);
		if (bytesSent == -1)
			error("sending");
		bytesTotal += bytesSent;
	}
}

/**
  * takes the response message and mallocs it into a new string
  * const int socketfd, the socket connected to the server
  */
static char *getData(SSL *socketfd) {
	char *buff = malloc(BUFFER * sizeof(char));
	ssize_t readSoFar;
	int limit = 0, bufferSize = BUFFER;
	while ((readSoFar = SSL_read(socketfd, buff, BUFFER)) > 0) {
		printf("%s", buff);
		memset(buff, 0, readSoFar);
//		printf("%zd bytes read\n", readSoFar);
//		limit += readSoFar;
//		if (limit > bufferSize * .9) {
//			printf("resizing buffer\n");
//			buff = resizeBuffer(buff, bufferSize);
//			bufferSize += bufferSize;
//		}
	}
	if (readSoFar == -1)
		error("read");
	buff[limit] = '\0';
	return buff;
}

SSL *createSSL(const int socketfd) {
	SSL_load_error_strings();
	SSL_library_init();
	SSL_CTX *ssl_ctx = SSL_CTX_new(SSLv23_client_method());
	SSL *conn = SSL_new(ssl_ctx);
	SSL_set_fd(conn, socketfd);
	if (SSL_connect(conn) == -1)
		error("ssl connect");
	return conn;
}

int main(const int argc, const char *argv[]) {
	if (argc < 3) {
		fprintf(stderr, "Incorrect usage: ./http <host> <path>\n");
		return EXIT_FAILURE;
	}
	char *http = createHTTPRequest(argv[2], argv[1]);
	struct sockaddr_in sockaddrin = connectToAddress(argv[1]);
	const int socketfd = createSocket();
	connectToServer(socketfd, sockaddrin);	
	SSL *conn = createSSL(socketfd);
	sendMsg(conn, http);
	char *response = getData(conn);
	printf("%s\n", response);
	SSL_shutdown(conn);
	close(socketfd);
	free(response);
	free(http);
	return EXIT_SUCCESS;
}

