# script to receive routing tables for demo 12/11/2023
# receives routing tables (including its own), parses them, converts into a graph
# to be run on pi7

import networkx as nx
import matplotlib.pyplot as plt
import matplotlib as mpl
import subprocess
from datetime import datetime, timedelta



#need server to receive routing table(s) from 4, 8, 9
#get my own routing table as well (7)

#string parsing to get info from routing table



#use routing tables to form netgraph

#create netgraph and display it on screen
#update netgraph when changes occur (every second)

nodes = [4,7,8,9]
elist = []
node_pos = {
  4: (50, 50),
  7: (100, 50),
  8: (50, 100),
  9: (100, 100)
}
fig = plt.figure()
#^ need to create this tuple from routing table
# differentiate AODV route from standard routes

G = nx.DiGraph()
G.add_nodes_from(nodes)

def parse_row(table_row):
    print("parsing: "+table_row)
    start = table_row.find('via')
    print("yo + " +table_row[start+14])
    destination_node = int(table_row[start+14])
    return destination_node

    # parse the table of node 7
def parse_my_table(table):
    #any other entries after row 3 must be routes
    print(len(table))
    for i in range(3, len(table)):
        elist.append((7, parse_row(table[i])))

# parse the table of node 4, 8, 9
def parse_table(table):
    #assume first row has which node this is
    row0 = table[2]
    source_node = int(row0[len(row0)-2])
    print(source_node)
    for i in range(3, len(table)):
        elist.append((source_node, parse_row(table[i])))

while True:
    my_table = subprocess.check_output(['ip', 'route'])
    my_table = my_table.decode('UTF-8').splitlines()



    G.clear_edges()
    elist = []

    parse_table(my_table)
    print(elist)

    #add edges between nodes
    G.add_edges_from(elist)
    print(G)
    nx.draw_networkx(G, with_labels=True, pos=node_pos)
    plt.ion()
    plt.show(block=False)
    plt.pause(5)
    plt.clf()