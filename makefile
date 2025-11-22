CC = gcc
CFLAGS = -g
LIBS = -lncurses -lpanel 

all: app

app: main.c
	$(CC) $(CFLAGS) main.c cJSON.c -o app $(LIBS)

clean:
	rm -f app

