CC = gcc

SRC = display.c main.c save.c

OBJ = $(SRC:.c=.o)

EXEC = jeu

all:
	$(CC) $(SRC) -o $(EXEC)

clean:
	rm -f *.o $(EXEC)