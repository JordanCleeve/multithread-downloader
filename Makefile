LIBS = -lpthread
CC = gcc -Iinclude -I./src
CFLAGS = -g -Wall --std=gnu99

.PHONY: default all clean

default: downloader queue_test http_test http_download
all: default

DEPS = src/http.h  src/queue.h  
OBJ = src/downloader.o  src/http.o src/queue.o 

QUEUE_OBJ = src/queue.o test/queue_test.o
HTTP_OBJ = src/http.o test/http_test.o
HTTP_DOWN_OBJ = src/http.o test/http_download.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

downloader: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

queue_test : $(QUEUE_OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)
	
http_test: $(HTTP_OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

http_download: $(HTTP_DOWN_OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)	

clean:
	-rm -f src/*.o test/*.o
	-rm -f downloader queue_test http_test http_download