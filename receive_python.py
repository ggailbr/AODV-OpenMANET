
# import the socketserver module of Python
import socket
import socketserver
import networkx as nx
import matplotlib.pyplot as plt
import matplotlib as mpl
import subprocess

#create globals for graph 
nodes = [4,7,8,9]
node_pos = {
  4: (50, 50),
  7: (100, 50),
  8: (50, 100),
  9: (100, 100)
}

G = nx.DiGraph()

# ['192.168.1.0/24 dev wlan0 proto kernel scope link src 192.168.1.4', '192.168.1.7 via 192.168.1.7 dev wlan0 proto static', '192.168.1.8 via 192.168.1.8 dev wlan0 proto static', '192.168.1.9 via 192.168.1.9 dev wlan0 proto static']
"""
['192.168.1.0/24 dev wlan0 proto kernel scope link src 192.168.1.4', '192.168.1.7 via 192.168.1.7 dev wlan0 proto static']
['192.168.1.0/24 dev wlan0 proto kernel scope link src 192.168.1.8', '192.168.1.7 via 192.168.1.7 dev wlan0 proto static']
['192.168.1.0/24 dev wlan0 proto kernel scope link src 192.168.1.9', '192.168.1.7 via 192.168.1.7 dev wlan0 proto static']
"""
# In this TCP server case - the request handler is derived from StreamRequestHandler

class MyTCPRequestHandler(socketserver.StreamRequestHandler):
    elist4 = []
    elist7 = []
    elist8 = []
    elist9 = []

    def parse_row(self, table_row):
        start = table_row.find('via')
        destination_node = int(table_row[start+14])
        return destination_node

    # parse the table of node 7
    def parse_my_table(self, table):
        G.remove_edges_from(self.elist7)
        self.elist7 = []
        #any other entries after row 3 must be routes
        for i in range(3, len(table)):
            self.elist7.append((7, self.parse_row(table[i])))
        G.add_edges_from(self.elist7)

    # parse the table of node 4, 8, 9
    def parse_table(self, table):
        #assume first row has which node this is
        row0 = table[0]
        source_node = int(row0[len(row0)-1])
        if source_node == 4:
            self.elist4 = []
            for i in range(1, len(table)):
                self.elist4.append((source_node, self.parse_row(table[i])))
        elif source_node == 8:
            self.elist8 = []
            for i in range(1, len(table)):
                self.elist8.append((source_node, self.parse_row(table[i])))
        elif source_node == 9:
            self.elist9 = []
            for i in range(1, len(table)):
                self.elist9.append((source_node, self.parse_row(table[i])))

    def handle(self):
        # Receive and print the data received from client
        route_lines = []
        # route_lines.append(self.client_address[0])
        msg = self.rfile.readline().strip().decode('utf-8')
        while(msg != "Acknowledged"):
            route_lines.append(msg)
            msg = self.rfile.readline().strip().decode('utf-8')

        # clear graph
        plt.clf()
        G.clear_edges()

        # parse new table(s)
        self.parse_table(route_lines)
        my_table = subprocess.check_output(['ip', 'route'])
        my_table = my_table.decode('UTF-8').splitlines()
        self.parse_my_table(my_table)

        print("edges of 7:")
        print(self.elist7)
        print("edges of 4:")
        print(self.elist4)
        print("edges of 8:")
        print(self.elist8)
        print("edges of 9:")
        print(self.elist9)

        # new graph
        #print(elist)
        G.add_edges_from(self.elist4)
        G.add_edges_from(self.elist7)
        G.add_edges_from(self.elist8)
        G.add_edges_from(self.elist9)

        nx.draw_networkx(G, with_labels=True, pos=node_pos)
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