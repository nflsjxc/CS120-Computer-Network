from Client import UDPClient
from Server import UDPServer
import time
import threading
import socket
from util import sendString, receiveString

RECEIVE = 0
SEND = 1
ICMP = 2
mode = RECEIVE
targetIP = ""
sendTime = 0
receiveTime = 0


def SendPingRequest(ip, num):
    client = UDPClient("127.0.0.1", 22111)
    global sendTime
    for i in range(num):
        message = ip + " " + str(i)
        sendString(message, client)
        snedTime = time.time()
        time.sleep(1)

def ReceivePingBack(num):
    server = UDPServer(22112)
    x = 0
    while True:
        message = receiveString(server)
        x += 1
        message.split(" ")
        if "TimeOut" in message:
            print("Request " + message[1] + " Time Out")
        else:
            print("Request " + message[0] + " IP = " + message[1] + " Time " + message[2] + "Payload : " + message[3])
        if x == num:
            break

if __name__ == "__main__":
    a = input("Please choose mode\n\n")
    if a[0] == 'R' or a[0] == 'r':
        mode = RECEIVE
    elif a[0] == 'S' or a[0] == 's':
        mode = SEND
    elif a[0] == 'I' or a[0] == 'i':
        mode = ICMP
        targetIP = input("Please input target IP\n\n")
    else:
        print("unknown instruction")

    if mode == RECEIVE:
        server = UDPServer(22112)
        out = ""
        lstData = ""
        while(True):
            data = server.receiveData()
            print("address = 192.168.1.1 ", "data : ", data, " port : ", server.port)
            if data == '#############':
                break
            if data != lstData:
                out += data
            lstData = data
        server.stopServer()
        with open("Receive.txt", "w") as f:
            end = 1
            print(out)
            while(True):
                if out[-end] == '#':
                    end += 1
                else:
                    break
            out = out[:-end]
            f.write(out)
            f.close()

    if mode == SEND:
        client = UDPClient("127.0.0.1", 22111)
        message = ""
        x = 0
        try:
            with open("input.txt") as f:
                for line in f:
                    for i in range(len(line)):
                        message += line[i]
                        x += 1
                        if x == 13:
                            client.sendData(message.encode('utf-8'))
                            message = ""
                            x = 0
                            time.sleep(0.2)
                f.close()
            if message != "":
                while len(message) < 13:
                    message += '#'
                client.sendData(message.encode('utf-8'))
            client.StopCilent()
        except KeyboardInterrupt:
            print("Node3 shut down by Keyboard")

    if mode == ICMP:
        sendPingRequest = threading.Thread(target=SendPingRequest, args=(targetIP, 10, ))
        receivePingBack = threading.Thread(target=ReceivePingBack, args=(10, ))
        receivePingBack.start()
        sendPingRequest.start()
        sendPingRequest.join()
        receivePingBack.join()