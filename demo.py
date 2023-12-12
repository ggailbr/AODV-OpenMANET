# script to receive routing tables for demo 12/11/2023
# receives routing tables (including its own), parses them, converts into a graph
# to be run on pi7

import networkx as nx
import matplotlib.pyplot as plt
import matplotlib as mpl
import subprocess

PORT = 26755

""" while socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind(("", PORT))
    s.listen()
    conn, addr = s.accept()
    with conn:
        print(f"Connected by {addr}")
        while True:
            data = conn.recv(1024)
            if not data:
                break
            conn.sendall(data) """


#need server to receive routing table(s) from 4, 8, 9
#get my own routing table as well (7)

#string parsing to get info from routing table



#use routing tables to form netgraph

#create netgraph and display it on screen
#update netgraph when changes occur (every second)

nodes = [4,7,8,9]
elist = []
#^ need to create this tuple from routing table
# differentiate AODV route from standard routes

G = nx.Graph()
G.add_nodes_from(nodes)
G.add_edges_from(elist)

# given an AODV route entry, parse who its going to
def parse_row(table_row):
    start = table_row.find('via')
    destination_node = table_row(1)

# parse the table of node 7
def parse_my_table(table):
    #assume third row has which node this is
    #row3 = table[2]
    #source_node = row3[len(row3)-2]

    #any other entries after row 3 must be routes
    for i in range(3, len(table)):
        elist.append(7, parse_row(table[i]))

# parse the table of node 4, 8, 9
def parse_table(table):
    #assume first row has which node this is
    row0 = table[0]
    source_node = row0[len(row0)-2]
    for i in range(3, len(table)):
        G.add_edge(source_node, parse_row(table[i]))



my_table = subprocess.check_output(['ip', 'route'])
my_table = my_table.decode('UTF-8').splitlines()
print(parse_table(my_table))
#del my_table[0]
#del my_table[0]
#del my_table[0]
#print(my_table)

#add edges between nodes

print(G)
nx.draw(G)