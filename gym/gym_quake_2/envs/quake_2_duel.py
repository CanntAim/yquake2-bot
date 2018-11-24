import socket
import sys

class Quake2DuelEnv(gym.Env):
    metadata = {'render.modes': ['human']}

    def __init__(self):
        # Global variables
        self.frags = 0
        self.observations = None
        
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
        self.observations = self.get_state()
        reward = self._get_reward()
        episode_over = self.observations["done"]
        return self.observations, reward, episode_over, {}

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
            data = message.encode("utf-8").split(",")
            self._parse(data)
            return data
        except:
            print('closing socket', file=sys.stderr)
            sock.close()

    def _get_reward(self):
        if self.observations[0] == "player" and self.observations[9] > self.frags:
            return 1

    def seed(self, seed):
        pass

    def _parse(self, data):
        length_player = 14
        length_entity = 7
        length_sound = 5
        obs = []
        if data[0] == "sound":
            obs =+ data[1:]
        elif data[0] == "player":
            obs =+ length_entity*[0]
            obs =+ data[2:]
        if len(obs) > 10000:
            pass
        else:
            obs =+ [0]
            
                
                
                
