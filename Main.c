#include <stdio.h>
#include "Http.h"


header *createHeader(const char *host, const char *url) {
	header *newHeader = malloc(sizeof(header));
	newHeader->host = strdup(host);
	newHeader->url = strdup(url);
	newHeader->next = NULL;
	return newHeader;
}

int main() {
	char *line = NULL, *host, *path;
	size_t linecap = 0;
	ssize_t linelen;
	header *head = NULL, *oldHeader;
	FILE *file = fopen("pathList.txt", "r");
	while ((linelen = getline(&line, &linecap, file)) != EOF) {
		line[linelen - 1] = '\0';
		host = line;
		for (path = host; *path != ' '; ++path);
		*path++ = '\0';
		oldHeader = head;
		head = createHeader(host, path);
		head->next = oldHeader;
	}
	fclose(file);
	httpProduct(head);
	return 0;
}
