

all: http smtp

http: Http.c
	gcc -Wall Http.c -o http -lssl -lcrypto -L/usr/local/opt/openssl/lib -I/usr/local/opt/openssl/include

smtp: Smtp.c
	gcc -Wall Smtp.c -o smtp
