import socket
import sys

class Quake2DuelEnv(gym.Env):
    metadata = {'render.modes': ['human']}

    def __init__(self):
        # Create a UDS socket
        sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

        # Connect the socket to the port where the server is listening
        server_address = "../../../quake_socket"
        print("connecting to %s" % server_address, file=sys.stderr)
        try:
            sock.connect(server_address)
        except socket.error as msg:
            print(msg, file=sys.stderr)

        try:
            message = 'Connected to server.'
            print('sending "%s"' % message, file=sys.stderr)
            sock.sendall(message.encode())
        except:
            print('closing socket', file=sys.stderr)
            sock.close()


    def _step(self, action):
        self._take_action(action)
        self.status = self.env.step()
        reward = self._get_reward()
        ob = self.get_state()
        episode_over = self.status != hfo_py.IN_GAME
        return ob, reward, episode_over, {}

    def _reset(self):
        pass
    
    def _render(self, mode='human', close=False):
        pass

    def _take_action(self, action):
        try:
            message = 'Tell server what to do.'
            sock.send(message.encode())
        except:
            print('closing socket', file=sys.stderr)
            sock.close()

    def _get_state(self):
        try:
            message = sock.recv(10000)
            print("\n")
            print('received "%s"' % message, file=sys.stderr)
        except:
            print('closing socket', file=sys.stderr)
            sock.close()

    def _get_reward(self):
        pass

    def seed(self, seed):
        pass
