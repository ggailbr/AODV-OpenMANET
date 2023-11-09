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
	make testbed
	$(CC) $(CFLAGS) $^ -o ./Build/$@ -ltestbed $(LIBPATH) -pthread -lnetfilter_queue

debug: AODV.c $(SOURCES)
	make testbed
	$(CC) $(CFLAGS) -DDEBUG $^ -o ./Build/AODV.out -ltestbed $(LIBPATH) -pthread -lnetfilter_queue

remote:
	cd MANET-Testbed; git pull origin main; make

testbed:
	cd MANET-Testbed; make

run: AODV.out
	sudo LD_LIBRARY_PATH=./MANET-Testbed:$LD_LIBRARY_PATH ./Build/AODV.out

clean:
	rm -rf Build/*


# iptables -t mangle -A PREROUTING -m mac --mac-source 48:5d:60:7e:bb:8f -j DROP