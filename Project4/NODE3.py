import socket
import time
from Client import UDPClient, TCPClient
from Server import UDPServer, TCPServer

SEND = 0
RECEIVE = 1
mode = SEND
node2Ip = "127.0.0.1"

class Node3:
    def __init__(self, ip, port, isClient):
        if isClient:
            self.client = UDPClient(ip, port)
        else:
            self.server = UDPServer(port)
        self.receivedMessageNum = 0
        self.sentMessageNum = 0
    
    def receiveFromNode2(self):
        try:
            with open("Node3tmp/Received.txt", "w") as f:
                while True:
                    data = self.server.receiveData()
                    address = self.server.addr
                    if data == "Exit":
                        print("Received all data!")
                        f.close()
                        break
                    else:
                        f.write(data)
                        print("address : ", address, " data : ", data)
        except KeyboardInterrupt:
            print("Node3 Shut down by keyboard")

    def sendToNode2(self):
        try:
            with open("input.txt") as f:
                for line in f:
                    print(line)
                    self.client.sendData(line.encode('utf-8'))
                    time.sleep(0.2) # wait for node2 to node1
                f.close()
            self.client.StopCilent()
        except KeyboardInterrupt:
            print("Node3 Shut down by keyboard")


if __name__ == "__main__":
    if mode == RECEIVE:
        node3 = Node3(node2Ip, 22111, False)
        node3.receiveFromNode2()
    elif mode == SEND:
        node3 = Node3(node2Ip, 22111, True)
        node3.sendToNode2()