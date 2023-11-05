CC = gcc
CFLAGS = -Wall 
CFLAGS += -IMANET-Testbed/
CFLAGS += -IDataStructures/
CFLAGS += -IMessages/
CFLAGS += -I./
SOURCES := $(wildcard DataStructures/*.c)
SOURCES += $(wildcard Messages/*.c)
LIBPATH = -L./MANET-Testbed
TEST_PATH = DataStructure/data_structure


AODV.out: AODV.c $(SOURCES)
	$(CC) $(CFLAGS) $^ -o ./Build/$@ -ltestbed $(LIBPATH) -pthread -lnetfilter_queue

debug: AODV.c $(SOURCES)
	$(CC) $(CFLAGS) -DDEBUG $^ -o ./Build/AODV.out -ltestbed $(LIBPATH) -pthread -lnetfilter_queue

testbed:
	cd MANET-Testbed; git pull origin main; make

run: AODV.out
	sudo LD_LIBRARY_PATH=./MANET-Testbed:$LD_LIBRARY_PATH ./Build/AODV.out

clean:
	rm -rf Build/*