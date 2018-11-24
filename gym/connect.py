import socket
import sys

# Helper functions

def parse(data):
    length_player = 14
    length_entity = 7
    length_sound = 5
    obs = []
    if data[0] == "sound":
        obs += data[1:]
    elif data[0] == "player":
        obs += length_entity*[0]
        obs += data[2:]
    if len(obs) > 10000:
        pass
    else:
        obs += [0]

# Create a UDS socket
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = "../quake_socket"
print("connecting to %s" % server_address, file=sys.stderr)

try:
    sock.connect(server_address)
except socket.error as msg:
    print(msg, file=sys.stderr)

try:
    # Send data
    message = 'Connected to server.'
    print('sending "%s"' % message, file=sys.stderr)
    sock.sendall(message.encode())

    while True:
        data = sock.recv(10000)
        data_list = data.decode("utf-8").split(",")
        print("\n")
        print('received "%s"' % data, file=sys.stderr)
        parse(data_list)
        message = 'Tell server what to do.'
        sock.send(message.encode())

finally:
    print('closing socket', file=sys.stderr)
    sock.close()

