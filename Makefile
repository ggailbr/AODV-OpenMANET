CC ?= gcc
CFLAGS = -Wall 
CFLAGS += -IMANET-Testbed/
CFLAGS += -IDataStructures/
CFLAGS += -IMessages/
CFLAGS += -DDEBUG
TEST_PATH = DataStructure/data_structure

AODV.out: AODV.c
	$(CC) $(CFLAGS) -o ./Build/$@ $^



clean:
	rm -rf Build/