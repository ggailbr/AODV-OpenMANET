CC ?= gcc
CFLAGS = -Wall 
CFLAGS += -I../
CFLAGS += -I../MANET-Testbed
CFLAGS += -I../DataStructures
CFLAGS += -DDEBUG
rerr.out: rerr.c rerr_test.c
	$(CC) $(CFLAGS) -o $@ $^

rreq.out: rreq.c rreq_test.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.out