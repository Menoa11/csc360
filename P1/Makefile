CC = gcc
CFLAGS = -Wall -g
TARGET = ssi

OBJS = main.o foreground.o background.o builtin.o signal_handler.o kill_process.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c ssi.h
	$(CC) $(CFLAGS) -c main.c

foreground.o: foreground.c ssi.h
	$(CC) $(CFLAGS) -c foreground.c

background.o: background.c ssi.h
	$(CC) $(CFLAGS) -c background.c

builtin.o: builtin.c ssi.h
	$(CC) $(CFLAGS) -c builtin.c

signal_handler.o: signal_handler.c ssi.h
	$(CC) $(CFLAGS) -c signal_handler.c

kill_process.o: kill_process.c ssi.h
	$(CC) $(CFLAGS) -c kill_process.c

clean:
	rm -f *.o $(TARGET)
