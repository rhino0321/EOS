CC=gcc
all : game guess
guess: guess.o
	$(CC) -o guess guess.o 
	
guess.o: guess.c
	$(CC) -c -o guess.o guess.c
	
game: game.o
	$(CC) -o game game.o 
	
game.o: game.c
	$(CC) -c -o game.o game.c
	
clean:
	rm -f game.*o
	rm -f guess.*o
