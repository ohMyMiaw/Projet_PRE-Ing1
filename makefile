CC = gcc
CFLAGS = -Wall -g
SRC = fichiers/display.c fichiers/items.c fichiers/patients.c fichiers/dentist.c fichiers/save.c fichiers/main.c
TARGET = game

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
