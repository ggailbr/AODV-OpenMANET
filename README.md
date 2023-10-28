## File Structure

``` bash
.
├── AODV.c
├── AODV.h
├── Build
│   └── aodv
├── DataStructures
│   ├── data_structure.c
│   ├── data_structure.h
│   ├── linked_list.c
│   ├── linked_list.h
│   ├── routing_table.c
│   ├── routing_table.h
│   ├── routing_test.c
│   ├── safe.c
│   ├── safe.h
│   ├── thread_funcs.c
│   └── thread_funcs.h
├── debug.h
├── Makefile
├── *MANET-Testbed
├── Messages
│   ├── rerr.c
│   ├── rerr.h
│   ├── rrep.c
│   ├── rrep.h
│   ├── rreq.c
│   ├── rreq.h
│   ├── send_messages.c
│   └── send_messages.h
└── README.md
```
## Files

`AODV.c/h` : Defines the parameters and routines that are used to run the AODV routing protocol. These should be as high level as possible and simple describe the routines needed for AODV operation

`Build/` : This stores the executable routing protocol

`DataStructures/` : Contains the various data structures and their required functions.

`-data_structure.c/h` : The data structure used to contain the routing table. It is implemented with as little information about the protocol as possible. It is a static array for hardware testing, but can be shifted to a hash table or more robust system seamlessly.

`-linked_list.c/h` : A linked list for storing information. Only storing uint32 so is hardcoded, but can be made more dynamic.

`-routing_table.c/h` : Provides base functionality for interacting with the routing table

`-safe.c/h` : Simple mutex grab and release functions for accessing global variables

`time_funcs.c/h` : Functions for manipulating timespec structures and using ms timers

`debug.h` : Defines some debugging statements

`Makefile` : How the whole routing protocol is going to build

`*Manet-Testbed` : Reference to the dynamic library

`Messages/` : Store functions for creating and manipulating message types

`-rerr.c/h` : Defines the structures needed for rerr messages

`-rrep.c/h` : Defines the structures needed for rrep messages

`-rreq.c/h` : Defines the structures needed for rreq messages

`-send_message.c/h` : The routines needed to send messages by the routing protocol

`README.md` : This