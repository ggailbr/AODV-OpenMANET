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

command:
	gcc -Wall -o ~/Documents/commands command_infrastructure.c -lpthread

testbed:
	cd MANET-Testbed; make

run: AODV.out
	sudo LD_LIBRARY_PATH=/home/pi/AODV-OpenMANET/MANET-Testbed:$LD_LIBRARY_PATH /home/pi/AODV-OpenMANET/Build/AODV.out

clean:
	rm -rf Build/*

# iptables -t mangle -A PREROUTING -m mac --mac-source e4:5f:01:47:56:f6 -j DROP     For 9
# iptables -t mangle -A PREROUTING -m mac --mac-source e4:5f:01:47:25:4f -j DROP     For 5
# iptables -t mangle -A PREROUTING -m mac --mac-source e4:5f:01:47:55:be -j DROP	 For 7
# iptables -t mangle -A PREROUTING -m mac --mac-source e4:5f:01:47:5d:e6 -j DROP	 For 8
# iptables -t mangle -A PREROUTING -m mac --mac-source 48:5d:60:7e:bb:8f -j DROP