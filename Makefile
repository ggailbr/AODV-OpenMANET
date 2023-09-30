CC ?= gcc
CFLAGS = -Wall 
CFLAGS += -IMANET-Testbed
CFLAGS += -IDataStructures/
CFLAGS += -IMessages/
CFLAGS += -DDEBUG

AODV.out: AODV.c
	$(CC) $(CFLAGS) -o ./Build/$@ $^

clean:
	rm -rf Build/