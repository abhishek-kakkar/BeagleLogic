CFLAGS += -std=c99 -g -O3 -Wall #-Werror
LDFLAGS += -lmosquitto

# Uncomment this to print out debugging info.
#CFLAGS += -DDEBUG

PROJECT=client

all: ${PROJECT}

client: client.o

client.o: Makefile

clean:
	rm -rf *.o ${PROJECT}
