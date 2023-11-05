CC = gcc
CFLAGS = -Wall 
CFLAGS += -IMANET-Testbed/
CFLAGS += -IDataStructures/
CFLAGS += -IMessages/
CFLAGS += -DDEBUG
CFLAGS += -I./
SOURCES := $(wildcard DataStructures/*.c)
SOURCES += $(wildcard Messages/*.c)
LIBPATH = -L./MANET-Testbed
TEST_PATH = DataStructure/data_structure


AODV.out: AODV.c $(SOURCES)
	$(CC) $(CFLAGS) $^ -o ./Build/$@ -ltestbed $(LIBPATH) -pthread -lnetfilter_queue

clean:
	rm -rf Build/