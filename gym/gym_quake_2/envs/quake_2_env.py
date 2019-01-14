import socket
import sys
import gym

import gym_quake_2.envs.bridge as bridge

class Quake2Env(gym.Env):    

    def __init__(self, socket, path, address, port, is_server, \
                 timescale, delimiter, size):
        # Global variables
        self.frags = 0
        self.address = address
        self.timescale = timescale
        self.delimiter = delimiter
        self.size = size
        
        self.episodes = 0
        self.steps = 0
        self.wins = 0

        self.action_space = self._action_space()
        self.observation_space = self._observation_space()

        # Create a connector
        self.connector = bridge.Connector(socket, path)

        if is_server:
            # Start server
            self.connector.start()
        else:
            # Connect to server
            self.connector.connect(address, port)
        
    def _step(self, action):
        self.steps += 1
        self.connector.send(self._command(action))
        observations = self.connector.receive(10000, ",")
        reward = self._compute_reward(observations)
        done = self._check_done()
        info = self._get_info()

        return observations, reward, done, info

    def _reset(self):
        pass
