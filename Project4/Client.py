import random
import socket
import time
import array

baseString = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
length = len(baseString)

def inCksum(packet):
    if len(packet) & 1:
        packet = packet + '\0'
    words = array.array('h', packet)
    sum = 0
    for word in words:
        sum += (word & 0xffff)
    sum = (sum >> 16) + (sum & 0xffff)
    sum = sum + (sum >> 16)
    return (~sum) & 0xffff

def randomString(n):
    ret = ""
    for i in range(n):
        ret += baseString[random.randint(0, length - 1)]
    return ret

class UDPClient:
    def __init__(self, ip, port):
        self.ip_port = (ip, port)
        self.clientsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    def sendData(self, data):
        # print(self.ip_port)
        self.clientsocket.sendto(data, self.ip_port)

    def StopCilent(self):
        self.sendData("#############".encode())
        print("Client exits success")

    def __del__(self):
        self.clientsocket.close()

class TCPClient:
    def __init__(self, ip, port):
        self.ip_port = (ip, port)
        self.clientsocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.clientsocket.connect((ip, port))
    
    def sendData(self, data):
        self.clientsocket.send(data)

    def StopCilent(self):
        self.sendData("Exit".encode())
        print("Client exits success")

    def __del__(self):
        self.clientsocket.close()

if __name__ == "__main__":
    client = UDPClient("127.0.0.1", 22111)
    try:
        while True:
            t = time.time()
            data = bytes(randomString(20), encoding="utf8")
            print(data.decode())
            client.sendData(data)
            time.sleep(0.05)
    except KeyboardInterrupt:
        client.StopCilent()
