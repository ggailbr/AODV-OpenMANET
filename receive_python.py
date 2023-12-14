
# import the socketserver module of Python
import socket
import socketserver
import networkx as nx
import matplotlib.pyplot as plt
import matplotlib as mpl
import subprocess

#8       9
#5       7
#create globals for graph 
nodes = [1,2,3,4]
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
# ['192.168.1.0/24 dev wlan0 proto kernel scope link src 192.158.1.4', '192.168.1.7 via 192.168.1.7 dev wlan0 proto static', '192.168.1.8 via 192.168.1.8 dev wlan0 proto static', '192.168.1.9 via 192.168.1.9 dev wlan0 proto static']
"""
['192.168.1.0/24 dev wlan0 proto kernel scope link src 192.168.1.4', '192.168.1.7 via 192.168.1.7 dev wlan0 proto static']
['192.168.1.0/24 dev wlan0 proto kernel scope link src 192.168.1.8', '192.168.1.7 via 192.168.1.7 dev wlan0 proto static']
['192.168.1.0/24 dev wlan0 proto kernel scope link src 192.168.1.9', '192.168.1.7 via 192.168.1.7 dev wlan0 proto static']
"""
# In this TCP server case - the request handler is derived from StreamRequestHandler

class MyTCPRequestHandler(socketserver.StreamRequestHandler):

    def parse_row(self, table_row):
        start = table_row.find('via')
        destination_node = int(table_row[start+14])
        return node_label[destination_node]

    # parse the table of node 7
    def parse_my_table(self, table):
        global elist5
        global elist7
        global elist8
        global elist9
        elist7 = []
        print("Received from 7: Clearing elist")
        #any other entries after row 3 must be routes
        for i in range(3, len(table)):
            elist7.append((node_label[7], self.parse_row(table[i])))

    # parse the table of node 5, 8, 9
    def parse_table(self, table):
        global elist5
        global elist7
        global elist8
        global elist9
        #assume first row has which node this is
        row0 = table[0]
        source_node = int(row0[len(row0)-1])
        print("Received from "+str(source_node)+": Clearing elist")
        if source_node == 5:
            elist5 = []
            for i in range(1, len(table)):
                elist5.append((node_label[source_node], self.parse_row(table[i])))
        elif source_node == 8:
            elist8 = []
            for i in range(1, len(table)):
                elist8.append((node_label[source_node], self.parse_row(table[i])))
        elif source_node == 9:
            elist9 = []
            for i in range(1, len(table)):
                elist9.append((node_label[source_node], self.parse_row(table[i])))

    def handle(self):
        global elist5
        global elist7
        global elist8
        global elist9

        # Receive and print the data received from client
        route_lines = []
        # route_lines.append(self.client_address[0])
        msg = self.rfile.readline().strip().decode('utf-8')
        while(msg != "Acknowledged"):
            route_lines.append(msg)
            msg = self.rfile.readline().strip().decode('utf-8')

        print(route_lines)

        # clear graph
        plt.clf()
        G.clear_edges()

        # parse new table(s)
        self.parse_table(route_lines)
        my_table = subprocess.check_output(['ip', 'route'])
        my_table = my_table.decode('UTF-8').splitlines()
        self.parse_my_table(my_table)

        """ print("edges of 7:")
        print(elist7)
        print("edges of 5:")
        print(elist5)
        print("edges of 8:")
        print(elist8)
        print("edges of 9:")
        print(elist9) """

        # new graph
        #print(elist)
        G.add_edges_from(elist5)
        G.add_edges_from(elist7)
        G.add_edges_from(elist8)
        G.add_edges_from(elist9)

        nx.draw_networkx(G, with_labels=True, pos=node_pos)
        fig = plt.gcf()
        fig.set_figwidth(2)
        fig.set_figheight(5)
        plt.ion()
        plt.show(block=False)
        plt.pause(1)
        plt.clf()

        
            

            

# Create a TCP Server instance

aServer = socketserver.TCPServer(("192.168.1.7", 26755), MyTCPRequestHandler)
aServer.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
aServer.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
 # Create a graph

# Listen for ever

aServer.serve_forever()