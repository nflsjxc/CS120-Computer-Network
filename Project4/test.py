from Client import UDPClient
from Server import UDPServer
from util import inCksum, sendString, receiveString
import socket
import struct
from scapy.all import *

NODE2IP = "127.0.0.1"

def packet_CallBack(packet):
    # client = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.getprotobyname('icmp'))
    ip_payload = bytes(packet['IP'].payload)
    ip_address = str(packet['IP'].src)
    # if ip_address != "172.20.10.6" and ip_address != "127.0.0.1":
    #     print("??????????")
    #     print(ip_address)
    #     print(">>>>>>")
    #     return
    packet.show()
    request_id = packet['ICMP'].id
    request_seq = packet['ICMP'].seq
    # ip_address = "127.0.0.1"
    print(ip_address)
    print(request_id)
    print(request_seq)
    # header = struct.pack('bbHHh', 0, 0, 0, request_id, request_seq)
    # data = ip_payload
    # packet = header + data
    # chkSum = inCksum(packet)
    # header = struct.pack('bbHHH', 0, 0, chkSum, request_id, request_seq)
    # icmp_data = header + data
    # client.sendto(icmp_data, (ip_address, 80))



if __name__ == "__main__":
    sniff(filter = 'icmp', prn = packet_CallBack, iface = "WLAN", count = 0)