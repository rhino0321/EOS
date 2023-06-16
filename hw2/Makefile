CC=gcc
all : hw2
lab5: hw2.o
	$(CC) -o hw2 hw2.o 
	
hw2.o: hw2.c
	$(CC) -c -o hw2.o hw2.c
	
clean:
	rm -f hw2.*o
