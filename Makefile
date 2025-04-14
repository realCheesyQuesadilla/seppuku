CC = gcc
CFLAGS = -Wall -Wextra -fPIC
LDFLAGS = -shared -lpthread -ldl

all: seppuku.so

seppuku.so: seppuku.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f seppuku.so

.PHONY: all clean