CC ?= gcc
CFLAGS = -Wall 
CFLAGS += -I../
CFLAGS += -I../MANET-Testbed
CFLAGS += -I../DataStructures
rerr.out: rerr.c rerr_test.c
	$(CC) $(CFLAGS) -DDEBUG -o $@ $^

rreq.out: rreq.c rreq_test.c
	$(CC) $(CFLAGS) -DDEBUG -o $@ $^

rrep.out: rrep.c rrep_test.c
	$(CC) $(CFLAGS) -DDEBUG -o $@ $^

clean:
	rm -f *.out