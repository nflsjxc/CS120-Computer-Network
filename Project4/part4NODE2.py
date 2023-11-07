from Client import UDPClient
from Server import UDPServer
from util import calculateChecksum, sendString, receiveString, isSpecialPayload
import socket
import struct
from scapy.all import *

NODE2IP = "172.20.10.9"
server = UDPServer(22112)
server.serversocket.settimeout(1)
client = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.getprotobyname('icmp'))
toCClient = UDPClient("127.0.0.1", 22111)

def packet_CallBack(packet):
    global client, toCClient, server
    if packet['ICMP'].type == 0:
        return
    ip_payload = bytes(packet['IP'].payload)[8:] # I don't know why need [8:] but it works! Don't change it unless you are surely know what you are doing.
    ip_address = str(packet['IP'].src)
    # print("?")
    if ip_address == NODE2IP:
        return
    # print(">")
    packet.show()
    default_Type = 0
    request_id = packet['ICMP'].id
    request_seq = packet['ICMP'].seq
    if isSpecialPayload(ip_payload):
        now = time.time()
        sendString("PING", toCClient)
        time.sleep(0.3)
        try:
            message = receiveString(server)
            print(message)
            if "PINGREPLY" not in message:
                return
        except socket.timeout:
            print("Time OUT!")
            return
    print("?")
    header = struct.pack('>bbHHH', 0, 0, 0, request_id, request_seq)
    data = ip_payload
    packet = header + data
    chkSum = calculateChecksum(packet)
    header = struct.pack('>bbHHH', 0, 0, chkSum, request_id, request_seq)
    icmp_data = header + data
    # print(icmp_data)
    client.sendto(icmp_data, (ip_address, 80))



if __name__ == "__main__":
    sniff(filter = 'icmp', prn = packet_CallBack, iface = "WLAN", count = 0)