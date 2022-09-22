#include <stdio.h>
#include "Http.h"

int main() {
	char *line = NULL, *host, *path;
	int sale;
	size_t linecap = 0;
	ssize_t linelen;
	FILE *file = fopen("pathList.txt", "r");
	while ((linelen = getline(&line, &linecap, file)) != EOF) {
		host = line;
		for (path = host; *path != ' '; ++path);
		*path++ = '\0';
		if ((sale = isOnSale(host, path)) != -1) {
			printf("item is on sale for %d\n", sale);
		}
	}
	fclose(file);
	return 0;
}
