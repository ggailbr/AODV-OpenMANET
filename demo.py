
# import the socketserver module of Python
import socket
import socketserver
import networkx as nx
import matplotlib.pyplot as plt
import matplotlib as mpl

#8       9
#5       7
#create globals for graph 
nodes = [1, 2, 3, 4]
node_pos = {
  2: (20, 30),
  1: (15, -50),
  3: (10, 70),
  4: (10, 130)
}
node_label = {
    5: 2,
    7: 1,
    8: 4,
    9: 3
}

G = nx.DiGraph()
elist5 = []
elist7 = []
elist8 = []
elist9 = []
G.add_node(10)

# clear graph
plt.clf()
G.clear_edges()

# new graph
elist7 = [(node_label[7],node_label[5]), (node_label[7],node_label[8]), (node_label[7],node_label[9])]
G.add_edges_from(elist5)
G.add_edges_from(elist7)
G.add_edges_from(elist8)
G.add_edges_from(elist9)

G.remove_node(10)
nx.draw_networkx(G, with_labels=True, pos=node_pos)

figure = plt.gcf()
figure.set_figwidth(2)
plt.show()