CC = gcc
SRC = $(wildcard fichiers/*.c)
OBJ = $(patsubst fichiers/%.c, build/%.o, $(SRC))
EXEC = build/jeu

all: build $(EXEC)

build:
	if not exist build mkdir build

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

build/%.o: fichiers/%.c
	$(CC) -c $< -o $@

clean:
	del /f $(OBJ) $(EXEC).exe