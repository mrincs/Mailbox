# This is the make file for Indiana University's Spring 2016 Advanced Operating
# Systems Course's (CS P536) extra credit assignment.

CC             = clang
CFLAGS         = -pthread -Wall -fpic
CFLAGS_RELEASE = -O3 -D NDEBUG
CFLAGS_DEBUG   = -O0 -g -D DEBUG
LFLAGS         = -pthread

EXEC = tring

SOURCES       = mailbox.c tring.c tring_thread.c queue.c util.c
HEADERS       = mailbox.h tring.h queue.h util.h
OBJECTS       = $(SOURCES:.c=.o)
DEBUG_OBJECTS = $(SOURCES:.c=.debug.o)

all: $(EXEC)

$(EXEC): $(OBJECTS) $(HEADERS)
	$(CC) -o $(EXEC) $(CFLAGS) $(CFLAGS_RELEASE) $(OBJECTS)

debug: $(DEBUG_OBJECTS) $(HEADERS)
	$(CC) -o $(EXEC) $(CFLAGS) $(CFLAGS_DEBUG) $(DEBUG_OBJECTS)


%.debug.o: %.c
	$(CC) -c $(CFLAGS) $(CFLAGS_DEBUG) $< -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $(CFLAGS_RELEASE) $< -o $@

graph: tring.dot
	dot -Tsvgz -o tring.svgz $<

.PHONY: clean
clean:
	rm -f $(EXEC) $(OBJECTS) $(DEBUG_OBJECTS)
