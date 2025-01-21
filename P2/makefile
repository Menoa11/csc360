CC = gcc

CFLAGS = -Wall -pthread

TARGET = mts

SRC = mts.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
