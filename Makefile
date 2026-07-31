CC = gcc
CFLAGS = -g
LDFLAGS = $(shell pkg-config --cflags --libs libsystemd)
SRC = src
BIN = bin
LIBS = libs
HEADERS = headers

all: clean mount_daemon run

mount_daemon: $(SRC)/mount_daemon.c $(LIBS)/*.o | $(BIN)
	$(CC) $^ $(CFLAGS)  -I$(HEADERS) $(LDFLAGS) -o $(BIN)/mount_daemon

$(LIBS)/ipc.o: $(HEADERS)/*.c 
	$(CC) -c $^ $(CFLAGS) -o $@ 

run: 
	$(BIN)/mount_daemon

clean:
	rm -rf $(BIN)/*
	rm -rf vgcore*

.PHONY: all clean run mount_daemon
