import socket
import random
import sys
import gym

import gym_quake_2.envs.bridge as bridge

class Quake2Env(gym.Env):    

    def __init__(self, socket, path, address, port, is_server, \
                 timescale):
        # Global variables
        self.frags = 0
        self.address = address
        self.port = port
        self.timescale = timescale
        
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

    def _command(self, action):
        return "test"
        
    def _step(self, action):
        self.steps += 1
        actions = ["attackup.", "forwardup.", \
                   "forwarddown.", "attackdown.", \
                   "backdown.", "backup."]
        self.connector.send(random.choice(actions))
        observations = self.connector.receive(10000, ",")
        reward = self._compute_reward(observations)
        done = self._check_done()
        info = self._get_info()

        return observations, reward, done, info

    def _action_space(self):
        """Returns a space object"""
        raise NotImplementedError

    def _observation_space(self):
        """Returns a space object"""
        raise NotImplementedError

    def _compute_reward(self, observations):
        """Returns a reward value"""
        print(observations)
        return 1 

    def _check_done(self):
        """Return whether done flag"""
        return False

    def _get_info(self):
        """Return statistics data"""
        return "test"

    def _reset(self):
        if self.address is None and self.port is None:
            self.connector.start()
        else:
            self.connector.connect(self.address, self.port)
