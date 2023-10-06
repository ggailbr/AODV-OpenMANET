CC ?= gcc
CFLAGS = -Wall 
CFLAGS += -I../

data_structure.out: data_structure.c data_structure_test.c
	$(CC) $(CFLAGS) -DDEBUG -o $@ $^

routing.out: routing_table.c routing_test.c
	$(CC) $(CFLAGS) -DDEBUG -o $@ $^

clean:
	rm -f *.out