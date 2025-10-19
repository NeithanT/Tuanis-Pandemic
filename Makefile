CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0`
TARGET = pandemic
SRC = src/main.c src/Interface/interface.c src/Structs/ListCreation.c src/Structs/Random/random.c src/Structs/Player/Player.c src/Play/Play.c src/Structs/Hash/Hash.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)