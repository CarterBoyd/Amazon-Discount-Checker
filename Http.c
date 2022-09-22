#include "Http.h"
#include <openssl/ssl.h>

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
  *
static inline char *resizeBuffer(char *oldBuffer, const int oldSize) {
	char *newBuffer = malloc(sizeof(char) * (oldSize * 2));
	memcpy(newBuffer, oldBuffer, oldSize);
	free(oldBuffer);
	return newBuffer;
}
*/

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
			"Accept-Language: en-US\r\n"
			"Connection: close\r\n"
			"\r\n";
	char output[HTTPBUFFER];
	int length = sprintf(output, msg, url, host); //url was the old one for path
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
	if (connect(socketfd, (struct sockaddr*) &sockaddrin, sizeof(sockaddrin)) == -1)
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
	while (bytesTotal < msgLen) {
		bytesSent = SSL_write(socketfd, msg + bytesTotal, msgLen - bytesTotal);
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
	while ((readSoFar = SSL_read(socketfd, buff + limit, bufferSize - limit)) > 0) {
		limit += readSoFar;
	}
	if (readSoFar == -1)
		error("read");
	buff[limit] = '\0';
	SSL_free(socketfd);
	return buff;
}

static SSL_CTX *createCTX() {
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
	SSL_CTX *ctx = SSL_CTX_new(TLS_client_method()); //depricated but if changed it will keep encrypted
	SSL_CTX_set_options(ctx, SSL_OP_NO_TLSv1_3);
//	SSL_CTX *ctx = SSL_CTX_new(TLSv1_2_client_method()); //depricated but if changed it will keep encrypted
	if (ctx == NULL)
		error("ctx");
	return ctx;
}

static SSL *createSSL(const int socketfd, SSL_CTX *ctx) {
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();	
	SSL *conn = SSL_new(ctx);
	if (conn == NULL)
		error("ssl_new(conn)");
	SSL_set_fd(conn, socketfd);
	if (SSL_connect(conn) == -1)
		error("ssl connect");
	return conn;
}

/**
  * When grabbing from amazons page the string will need the class
  * then it will look for "Save " from there a pointer will look for
  * when the % gets there and then it will isolate and return the
  * number
  *
  * @return -1 if there is no sale, or the actual percentage of
  *		products sale
  * NEEDS TO BE TESTED MORE
  * 404 error: works
  * 500 bad input:
  * 300 input:
  * 200 input with sale: works
  * 200 input with no sale:
  */
static const int isOnSale(const char *webpage) {
	const char *strInt = "<span class=\"delight-pricing-badge-label-text a-text-ellipsis\">";
	char *ptr, *save = strstr(webpage, strInt);
	if (save != NULL) {
		save = strstr(save, "Save ");
		if (save != NULL) {
			for (ptr = save; *ptr != '%'; ++ptr);
			*ptr = '\0';
			save += 5;
			return atoi(save);
		}
	}
	return -1;
}

/**
  * WARNING: do not use multiple websites at this moment
  * 
  * will run through the 
  */
void httpProduct(header *head) {
	header *currHead;
	SSL_library_init();
	char *http, *response;
	struct sockaddr_in sockaddrin;
	int socketfd, sale;
	SSL_CTX *ctx;
	SSL *conn;
	while (head != NULL) {
		sockaddrin = connectToAddress(head->host);
		socketfd = createSocket();
		connectToServer(socketfd, sockaddrin);
		ctx = createCTX();
		conn = createSSL(socketfd, ctx);
		http = createHTTPRequest(head->url, head->host);
		sendMsg(conn, http);
		response = getData(conn);
		sale = isOnSale(response);
		if (sale != -1)
			printf("Sale is: %d\n", sale);
		else
			fprintf(stderr, "could not find sale\n");
		currHead = head;
		head = head->next;
		free(currHead);
		free(response);
		free(http);
		SSL_shutdown(conn);
		close(socketfd);
	}
}

