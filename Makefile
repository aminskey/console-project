CC = gcc
CFLAGS = -g -lm -fPIC 
LDFLAGS = $(shell pkg-config --cflags --libs libsystemd)
SRC = ./src
BIN = ./bin
LIBS = ./libs
OBJS = ./obj
HEADERS = ./headers
HEADERSRC = ./headersrc
ALLSRC = $(wildcard ./headersrc/*.c)
OBJECTS = $(patsubst $(HEADERSRC)/%.c, $(OBJS)/%.o, $(ALLSRC)) 

all: clean mount_daemon

mount_daemon: $(SRC)/mount_daemon.c $(LIBS)/*.so | $(BIN)
	$(CC) $^ $(CFLAGS)  -I$(HEADERS) $(LDFLAGS) -o $(BIN)/mount_daemon

$(OBJS)/%.o: $(HEADERSRC)/%.c
	$(CC) -c $< $(CFLAGS) -I$(HEADERS) -o $@

$(OBJS): $(OBJECTS)

$(LIBS)/libipc.so: $(OBJS)/*.o 
	$(CC) -shared $^ -o $@

$(BIN)/serverExample: $(SRC)/server.c $(LIBS)/*.so | $(BIN)
	$(CC) $^ $(CFLAGS) -I$(HEADERS) -L$(LIBS) -o $@


clean:
	rm -rf $(BIN)/*
	rm -rf $(OBJS)/*
	rm -rf vgcore*

.PHONY: all clean mount_daemon
