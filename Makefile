

all: main http

http: Http.c
	gcc -Wall Http.c -lssl -lcrypto -L/usr/local/opt/openssl/lib -I/usr/local/opt/openssl/include

smtp: Smtp.c
	gcc -Wall Smtp.c -o smtp

main: http  main.c
	gcc -Wall main.c -o main Http.o -lssl -lcrypto -L/usr/local/opt/openssl/lib -I/usr/local/opt/openssl/include
