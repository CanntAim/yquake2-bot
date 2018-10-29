import socket
import sys

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
        print(data_list)
        message = 'Tell server what to do.'
        sock.send(message.encode())

finally:
    print('closing socket', file=sys.stderr)
    sock.close()

