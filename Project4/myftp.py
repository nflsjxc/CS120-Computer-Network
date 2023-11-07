from ftplib import all_errors, FTP

class ftpClient:
    def __init__(self):
        self.ftpClient = FTP()
    
    def CONT(self, host, port = 21):
        try:
            return self.ftpClient.connect(host, port)
        except all_errors as e:
            return str(e)

    def USER(self, username = "anonymous"):
        try:
            return self.ftpClient.sendcmd("USER " + username)
        except all_errors as e:
            return str(e)

    def PASS(self, password = "admin"):
        try:
            return self.ftpClient.sendcmd("PASS " + password)
        except all_errors as e:
            return str(e)
    
    def PWD(self):
        try:
            return self.ftpClient.sendcmd("PWD")
        except all_errors as e:
            return str(e)
    
    def CWD(self, path = ""):
        try:
            return self.ftpClient.cwd(path)
        except all_errors as e:
            return str(e)

    def PASV(self):
        try:
            return self.ftpClient.sendcmd("PASV")
        except all_errors as e:
            return str(e)

    def LIST(self, dirpath = ""):
        try:
            lines = []
            self.ftpClient.retrlines('LIST'+ dirpath, lines.append)
            return "\n".join(lines)
        except all_errors as e:
            return str(e)
    
    def RETR(self, dirpath):
        print("retr ",dirpath)
        try:
            with open(dirpath, "wb") as f:
                response = self.ftpClient.retrbinary("RETR " + dirpath, f.write, 1024)
            f.close()
            return response
        except all_errors as e:
            return

    def QUIT(self):
        try:
            return self.ftpClient.quit()
        except all_errors as e:
            return str(e)

if __name__ == "__main__":
    client = ftpClient()
    print(client.CONT("ftp.ncnu.edu.tw"))
    print(client.USER())
    print(client.PASS())
    print(client.PWD())
    print(client.CWD("pub"))
    print(client.PASV())
    print(client.LIST())
    print(client.RETR("robots.txt"))
    print(client.QUIT())
    while(True):
        pass