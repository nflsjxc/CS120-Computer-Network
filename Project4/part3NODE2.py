from Client import UDPClient
from Server import UDPServer
from util import receiveString, sendString, inCksum
import socket
import time
import struct
import array

def ReceiveFromPhy():
    server = UDPServer(22112)
    toCClient = UDPClient("127.0.0.1", 22111)
    client = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.getprotobyname('icmp'))
    client.settimeout(1)
    while True:
        data = receiveString(server)
        print(data)
        data = data.split(" ")
        targetIp = data[0]
        index = int(data[1])
        # targetIp = "www.baidu.com"
        # index = 1
        header = struct.pack('bbHHh', 8, 0, 0, index, 0)
        pl = time.time()
        data = struct.pack('d', pl)
        packet = header + data
        chkSum = inCksum(packet)
        header = struct.pack('bbHHh', 8, 0, chkSum, index, 0)
        icmp_data = header + data
        now = time.time()
        client.sendto(icmp_data, (targetIp, 80))
        try:
            while True:
                recv_packet, addr = client.recvfrom(1024)
                if addr == "172.20.10.6":
                    break
            latency = time.time() - now
            # print("LATENCY", latency)
            type, code, checkSum, packetId, sequence = struct.unpack("bbHHh", recv_packet[20:28])
            payload = struct.unpack('bbbbbbbb', recv_packet[28:36])
            print(addr[0], type, code, checkSum, packetId, sequence, payload)
            strPayLoad = ""
            for x in payload:
                print(x)
                strPayLoad += str(x)
            print(round(latency * 1000))
            retData = str(index) + " " + str(addr[0]) + " " + str(round(latency * 1000)) + " " + strPayLoad
            sendString(retData, toCClient)
        except socket.timeout:
            sendString("TimeOut " + str(index), toCClient)
        
        time.sleep(1.5)

if __name__ == "__main__":
    ReceiveFromPhy()