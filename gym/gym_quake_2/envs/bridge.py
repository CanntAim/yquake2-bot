import subprocess
import socket
import time
import sys

class Connector:
    def __init__(self, address, path):
        subprocess.Popen("./quake2", cwd=path)
        time.sleep(5)
        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        print("connecting to %s" % address, file=sys.stderr)
        try:
            self.sock.connect(address)
        except socket.error as msg:
            print(msg, file=sys.stderr)


    def send(self, message):
        try:
            print("sending '%s'" % message, file=sys.stderr)
            self.sock.sendall(message.encode())
        except:
            print("closing socket", file=sys.stderr)
            self.sock.close()

    def receive(self, size, delimiter):
        message = self.sock.recv(size)
        data = message.decode("utf-8").split(delimiter)
        return data

    def start(self):
        errored = False
        started = False
        send(self, "start server")
        while not started and not errored:
            if "error" == received(50, ",")[0]:
                errored = True
            elif "successfully started server":
                started = True

    def connect(address, port):
        errored = False
        connected = False
        send(self, "connect to server")
        while not connected and not errored:
            if "error" == received(50, ",")[0]:
                errored = True
            elif "successfully connected to server" == received(50, ","):
                connected = True
