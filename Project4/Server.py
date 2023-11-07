import socket

class UDPServer:
    def __init__(self, port):
        self.serversocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.serversocket.bind(("", port))
        self.port = port

    def receiveData(self):
        data, address = self.serversocket.recvfrom(1024)
        data = data.decode('utf8')
        self.addr = address
        return data
    
    def stopServer(self):
        print("Server exits success")
    
    def __del__(self):
        self.serversocket.close()

class TCPServer:
    def __init__(self, port):
        self.serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.serversocket.bind(("", port))
        self.serversocket.listen(5)
        self.clientsocket, self.addr = self.serversocket.accept()

    def receiveData(self):
        # self.clientsocket, self.addr = self.serversocket.accept()
        data = self.clientsocket.recv(1024)
        data = data.decode('utf8')
        return data
    
    def stopServer(self):
        print("Server exits success")
    
    def __del__(self):
        self.serversocket.close()
    
if __name__ == "__main__":
    server = UDPServer(22112)
    while True:
        data = server.receiveData()
        print("address : ", server.addr, " data : ", data)
        if data == "Exit":
            break
    server.stopServer()