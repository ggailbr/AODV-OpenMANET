import socket
import os
from pathlib import Path
import argparse
import binascii

mac_addresses_location = Path('/etc/ethers')
class TestCoordinator():
    ips = []
    hardware_addresses = {}
    def __init__(self):
        pass

    def reset_table():
        pass

    def exclude(self, host_names):
        '''
            Breif:
            -------
                Excludes the supplied host names from communication with 
                this node. It adds a rule to iptables for each IP which automatically
                drops packets associated with the mac address of the ip.
            
            Parameters:
            -----------
                - `host_name`:
                    A string list of host_names to exclude in iptables
        '''
        for host_name in host_names:
            try:
                ip_address = socket.gethostbyname(host_name)
                self.ips.append(ip_address)
            except socket.gaierror:
                print(f"Do not have an ip for {host_name}")
        with open("/etc/ethers","r") as hardware_addresses:
            hardware_list = [i.strip('\n').split(' ') for i in hardware_addresses.readlines()]
            for hardware_pair in hardware_list:
                self.hardware_addresses[hardware_pair[1]] = hardware_pair[0]
            for ip in self.ips:
                if ip in self.hardware_addresses.keys():
                    
                    print(self.hardware_addresses[ip])
        



if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--exclude', '-x', nargs='*', help='Hostnames to exclude')
    parser.add_argument('--deploy', '-d', type=list, help='Send the current folder to host(s) and build the testbed')
    args = parser.parse_args()
    print(args.exclude)
    test = TestCoordinator()
    test.exclude(args.exclude)
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.bind(("", 269))
        while True:
            data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
            print("received message: %s" % binascii.hexlify(data))
            print(addr)
