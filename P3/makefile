CC = gcc
CFLAGS = -Wall -Wextra -g -w
TARGETS = disklist diskinfo diskget diskput

all: $(TARGETS)

diskinfo: diskinfo.c sharedFunctions.c
	$(CC) $(CFLAGS) -o $@ diskinfo.c sharedFunctions.c

disklist: disklist.c sharedFunctions.c
	$(CC) $(CFLAGS) -o $@ disklist.c sharedFunctions.c

diskget: diskget.c sharedFunctions.c
	$(CC) $(CFLAGS) -o $@ diskget.c sharedFunctions.c

diskput: diskput.c sharedFunctions.c
	$(CC) $(CFLAGS) -o $@ diskput.c sharedFunctions.c

clean:
	rm -f $(TARGETS)
	rm -f *.o

.PHONY: all clean