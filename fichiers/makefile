CC = gcc
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
EXEC = jeu

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

%.o: %.c
	$(CC) -c $< -o $@

clean:
	del /f $(OBJ) $(EXEC).exe