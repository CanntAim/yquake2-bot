import subprocess
import socket
import time
import sys
import os

class Connector:
    def __init__(self, address, path, headless):
        if not headless:
            subprocess.Popen("./quake2", cwd=path, stdout=open( 'quake.log', 'w'))
        else:
            os.environ["SDL_VIDEODRIVER"] = "dummy"
            os.environ["SDL_AUDIODRIVER"] = "dummy"
            subprocess.Popen(['xvfb-run', '-a', '--server-args="0 1024x768x24"', './quake2'], cwd=path, stdout=open( 'quake.log', 'w'))
        print("Waiting 10 seconds for game to start before opening socket connection.")
        time.sleep(10)
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

    def start(self, address, level, headless):
        errored = False
        started = False
        self.send("start server, {}, {},{}".format(level, address, \
                                                   headless))
        while not started and not errored:
            if "error" == self.receive(50, ",")[0]:
                errored = True
                print("obtained server failure notice")
            else:
                started = True
                print("acknowledged server started")

    def connect(self, address, port, headless):
        errored = False
        connected = False
        self.send("connect to server, blank, {}:{},{}".format(address, \
                                                              port, headless))
        while not connected and not errored:
            if "error" == self.receive(50, ",")[0]:
                errored = True
                print("obtained server failure notice")
            else:
                connected = True
                print("acknowledge connected to server")
