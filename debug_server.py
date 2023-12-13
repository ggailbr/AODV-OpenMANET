
# import the socketserver module of Python
import socket
import socketserver

 

# Create a Request Handler

# In this TCP server case - the request handler is derived from StreamRequestHandler

class MyTCPRequestHandler(socketserver.StreamRequestHandler):

    def handle(self):

        # Receive and print the data received from client
        route_lines = []
        # route_lines.append(self.client_address[0])
        msg = self.rfile.readline().strip().decode('utf-8')
        while(msg != "Acknowledged"):
            route_lines.append(msg)
            msg = self.rfile.readline().strip().decode('utf-8')
        print(route_lines)
        
            

            

# Create a TCP Server instance

aServer = socketserver.TCPServer(("192.168.1.7", 26755), MyTCPRequestHandler)
aServer.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
aServer.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
 # Create a graph

# Listen for ever

aServer.serve_forever()