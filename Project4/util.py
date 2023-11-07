import time
import array
import struct

def sendString(data, client):
    x = 0
    message = ""
    for i in range(len(data)):
        message += data[i]
        x += 1
        if x == 13:
            client.sendData(message.encode('utf-8'))
            x = 0
            message = ""
            time.sleep(0.02)
    if message != "":
        while len(message) < 13:
            message += "^"
        client.sendData(message.encode('utf-8'))
    client.sendData("^^^^^^^^^^^^^".encode('utf-8'))

def receiveString(server):
    out = ""
    lstData = ""
    while(True):
        data = server.receiveData()
        print("data : ", data, " port : ", server.port)
        if data == '^^^^^^^^^^^^^':
            break
        if data != lstData:
            out += data
        lstData = data
    end = 0
    # print(out)
    while(True):
        try:
            if out[end-1] == '^':
                end -= 1
            else:
                break
        except:
            end=0
    if end != 0:
        out = out[:end]
    return out

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

def calculateChecksum(data) :
    checksum = 0
    count = (len(data) // 2) * 2
    i = 0

    while i < count:
        temp = data[i + 1] * 256 + data[i]
        checksum = checksum + temp
        checksum = checksum & 0xffffffff 
        i = i + 2

    if i < len(data):
        checksum = checksum + data[len(data) - 1]
        checksum = checksum & 0xffffffff

    # 32-bit to 16-bit
    checksum = (checksum >> 16) + (checksum & 0xffff)
    checksum = checksum + (checksum >> 16)
    checksum = ~checksum
    checksum = checksum & 0xffff

    checksum = checksum >> 8 | (checksum << 8 & 0xff00)
    return checksum

def isSpecialPayload(ip_payload, pattern = "114514"):
    x = struct.unpack('>28h', ip_payload)
    s = ""
    for i in x:
        s += str(hex(i)[2:])
    s += s
    if pattern in s:
        return True
    return False