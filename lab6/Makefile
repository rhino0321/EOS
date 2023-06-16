CC=gcc
all : server client
client: client.o
	$(CC) -o client client.o 
	
client.o: client.c
	$(CC) -c -o client.o client.c
	
server: server.o
	$(CC) -o server server.o 
	
server.o: server.c
	$(CC) -c -o server.o server.c
	
clean:
	rm -f server.*o
	rm -f client.*o
