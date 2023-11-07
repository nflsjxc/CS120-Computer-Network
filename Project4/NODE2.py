import socket
import time
import os
import array
import struct
from Server import UDPServer, TCPServer
from Client import UDPClient, TCPClient

SEND = 0
RECEIVE = 1
ICMP = 2
mode = SEND
node3Ip = "10.20.167.54"

class Node2:
    def __init__(self, ip, port, isClient, ICMPRequest):
        if ICMPRequest:
            start = time.time()
            now = time.time()
            while True:
                now = time.time()
                if(now - start > 3):
                    print("No Target Ip! Will Exit")
                    break
                if os.path.exists("Node2tmp/TargetIpGet.txt"):
                    with open("Node2tmp/TargetIp.txt") as f:
                        self.targetIp = f.read()
                        print("TargetIP: ", self.targetIp)
                        f.close()
                        break
                
            if now - start <= 3:
                self.Sock = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.getprotobyname('icmp'))
                self.Sock.settimeout(1)

        elif isClient:
            self.client = UDPClient(ip, port)
        else:
            self.server = UDPServer(port)
        self.receivedMessageNum = 0
        self.sentMessageNum = 0
        self.ICMPRequestNum = 0

    def sendToNode3(self):
        # This function used to transmit data from node1 to node2
        try:
            while True:
                if os.path.exists("C:\\Users\\Cabbage\\Desktop\\Network-debug\\Data\\OutputOk.txt"):
                    with open("C:\\Users\\Cabbage\\Desktop\\Network-debug\\Data\\Output1.txt") as f:
                        # data = f.read().encode()
                        # self.client.sendData(data)
                        # print(data.decode())
                        # time.sleep(0.1)
                        # self.receivedMessageNum += 1
                        for line in f:
                            self.client.sendData(line.encode('utf-8'))
                            print(line)
                            time.sleep(0.2)
                        f.close()
                        self.client.sendData("Exit".encode())
                        break
        except KeyboardInterrupt:
            print("Node2 shut down by keyboard")
            self.client.sendData("Exit".encode())
    
    def receiveFromNode3(self):
        try:
            while True:
                data = self.server.receiveData()
                address = self.server.addr
                if data == "Exit" :
                    print("Received all data!")
                    break
                else:
                    with open("Node2tmp/PacketReceivedFromNode3" + str(self.receivedMessageNum) + ".txt", "w") as f:
                        f.write(data)
                        print("address : ", address, " data : ", data)
                        f.close()
                    with open("Node2tmp/" + str(self.receivedMessageNum) + "ReceivedFromNode3.txt", "w") as f:
                        f.write('1')
                        f.close()
                        
                    self.receivedMessageNum += 1
        except KeyboardInterrupt:
            print("Node2 shut down by keyboard")
            self.server.stopServer()

    def pingOthers(self):
        try:
            while True:
                if os.path.exists("Node2tmp/ICMPRequest" + str(self.ICMPRequestNum) + ".txt"):
                    header = struct.pack('bbHHh', 8, 0, 0, self.ICMPRequestNum, 0)
                    data = struct.pack('d', time.time())
                    packet = header + data
                    chkSum = inCksum(packet)
                    header = struct.pack('bbHHh', 8, 0, chkSum, self.ICMPRequestNum, 0)
                    icmp_data = header + data
                    start = time.time()
                    self.Sock.sendto(icmp_data, (self.targetIp, 80))
                    self.ICMPRequestNum += 1
                    try:
                        recv_packet, addr = self.Sock.recvfrom(1024)
                        end = time.time()
                        type, code, checkSum, packe_ID, sequence = struct.unpack("bbHHh", recv_packet[20:28])
                        print("Header : ", type, code, checkSum, packe_ID, sequence)
                        print("Time : ", end - start)
                    except socket.timeout:
                        print("Time out!")
        except KeyboardInterrupt:
            print("Node2 shut down by keyboard")

if __name__ == "__main__":
    if mode == SEND:
        node2 = Node2(node3Ip, 22111, True, False)
        node2.sendToNode3()
    elif mode == RECEIVE:
        node2 = Node2(node3Ip, 22111, False, False)
        node2.receiveFromNode3()
    elif mode == ICMP:
        node2 = Node2(node3Ip, 22111, False, True)
        node2.pingOthers()