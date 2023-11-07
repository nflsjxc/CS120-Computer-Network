from Client import UDPClient, randomString
from Server import UDPServer
from util import sendString
import threading
import time

outputBuffer = ""
outLen = 0
sentLen = 0
inputBuffer = ""
inLen = 0
receiveLen = 0

def ReceiveFromPhy():
    server = UDPServer(22112)
    global outputBuffer, outLen
    while True:
        data = server.receiveData()
        print("address : ", server.addr, " data : ", data)
        outputBuffer += data
        outLen += 13
        if data == "#############":
            break
    server.stopServer()

def SendToPhy():
    client = UDPClient("127.0.0.1", 22111)
    global receiveLen, inLen, inputBuffer
    sendString("10.20.210.10", client)
    try:
        while True:
            if receiveLen < inLen:
                inData = bytes(inputBuffer[receiveLen:receiveLen+13], encoding="utf8")
                receiveLen += 13
                print(inData.decode(), end="")
                client.sendData(inData)
                time.sleep(0.05)
            time.sleep(0.05)
    except KeyboardInterrupt:
        client.StopCilent()

def SendToOut():
    client = UDPClient("10.20.209.144", 22333)
    global outputBuffer, outLen, sentLen
    try:
        while True:
            try:
            # data = bytes(randomString(13), encoding="utf8")    
            # client.sendData(data)
                if sentLen < outLen:
                    outData = bytes(outputBuffer[sentLen:sentLen+13], encoding="utf8")
                    sentLen += 13
                    print(outData.decode())
                    client.sendData(outData)
                time.sleep(0.02)
            except KeyboardInterrupt:
                client.StopCilent()
                break
    except KeyboardInterrupt:
        client.StopCilent()

def ReceiveFromOut():
    server = UDPServer(11111)
    global inputBuffer, inLen
    while True:
        data = server.receiveData()
        # print("address : ", server.addr, " data : ", data)
        inputBuffer += data
        inLen += 13
        if data == "#############":
            break
    # server.stopServer()

if __name__ == "__main__":
    receiveFroPhy = threading.Thread(target=ReceiveFromPhy)
    sendToOut = threading.Thread(target=SendToOut)
    receiveFromOut = threading.Thread(target=ReceiveFromOut)
    sendToPhy = threading.Thread(target=SendToPhy)
    sendToPhy.start()
    sendToOut.start()
    receiveFroPhy.start()
    receiveFromOut.start()
    sendToOut.join()
    receiveFroPhy.join()
    sendToPhy.join()
    receiveFromOut.join()