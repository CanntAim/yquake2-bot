"""
Module
"""
import math
import random
import gym
import numpy as np
import tensorflow.compat.v1 as tf
import gym_quake_2.envs.bridge as bridge

tf.disable_v2_behavior()

GAMMA = 0
LAMBDA = 0
MIN_EPSILON = 0
MAX_EPSILON = 0

class Runner():
    def __init__(self, initial_value, episodes, environment):
        self._eps = 0
        self._episodes = episodes
        self._environment = environment
        self._environment.seed(initial_value)
        self._model = Model(10, 10, 10)
        self._memory = Memory(50000)
        self._reward_store = []

    def _choose_action(self, state, session):
        if random.random() < self._eps:
            return random.randint(0, self._model.num_actions - 1)
        return np.argmax(self._model.predict_one(state, session))

    def _replay(self, session):
        batch = self._memory.sample(self._model.batch_size)
        states = np.array([val[0] for val in batch])
        next_states = np.array([(np.zeros(self._model.num_states)
                                 if val[3] is None else val[3]) for val in batch])

        q_s_a = self._model.predict_batch(states, session)
        q_s_a_d = self._model.predict_batch(next_states, session)

        states = np.zeros((len(batch), self._model.num_states))
        actions = np.zeros((len(batch), self._model.num_actions))
        for idx, tup in enumerate(batch):
            state, action, reward, next_state = tup[0], tup[1], tup[2], tup[3]
            current_q = q_s_a[idx]
            if next_state is None:
                current_q[action] = reward
            else:
                current_q[action] = reward + GAMMA * np.amax(q_s_a_d[idx])
            states[idx] = state
            actions[idx] = current_q
        self._model.train_batch(session, states, actions)

    def run(self):
        with tf.Session() as session:
            self._model._var_init.run()
            state = self._environment._reset()
            tot_reward = 0
            max_x = -100
            while self._episodes:
                action = self._choose_action(state, session)
                next_state, reward, done, info = self._environment._step(action)
                if done:
                    next_state = None
                self._memory.add_sample((state, action, reward, next_state))

                self._eps = MIN_EPSILON + (MAX_EPSILON - MIN_EPSILON) \
                            * math.exp(-LAMBDA * self._environment._steps)

                state = next_state
                tot_reward += reward

                if done:
                    self._reward_store.append(tot_reward)
                    break

class Quake2Env(gym.Env):
    def __init__(self, socket, path, address, port, level, is_server, \
                 timescale, headless):
        self._address = address
        self._port = port
        self._level = level
        self._is_server = is_server
        self._timescale = timescale
        self._headless = headless
        self._steps = 0

        self._action_space = self._action_space()
        self._observation_space = self._observation_space()
        self._connector = bridge.Connector(socket, path, headless)

    def _step(self, action):
        self._steps += 1
        actions = ["attackup.", "forwardup.", \
                   "forwarddown.", "attackdown.", \
                   "backdown.", "backup."]
        self._connector.send(action)
        observations = self._connector.receive(10000, ",")
        reward = self._compute_reward(observations)
        done = self._check_done()
        info = self._get_info()

        return observations, reward, done, info

    def _compute_reward(self, observations):
        """Returns a reward value"""
        print(observations)
        return 1

    def _action_space(self):
        """Returns an action space object"""
        raise NotImplementedError

    def _observation_space(self):
        """Returns an observation space object"""
        raise NotImplementedError

    def _check_done(self):
        """Return whether done flag"""
        return False

    def _get_info(self):
        """Return statistics data"""
        return "test"

    def _reset(self):
        if self._is_server:
            self._connector.start(self._address, self._level, \
                                 self._headless)
        else:
            self._connector.connect(self._address, self._port, \
                                   self._headless)
        return np.zeros((10,), dtype=int)

class Memory:
    def __init__(self, max_memory):
        self._max_memory = max_memory
        self._samples = []

    def add_sample(self, sample):
        """
        Add sample to memory.
        """
        self._samples.append(sample)
        if len(self._samples) > self._max_memory:
            self._samples.pop(0)

    def sample(self, no_samples):
        """
        Sample the memory.
        """
        if no_samples > len(self._samples):
            return random.sample(self._samples, len(self._samples))
        return random.sample(self._samples, no_samples)

class Model:
    def __init__(self, num_states, num_actions, batch_size):
        self.num_states = num_states
        self.num_actions = num_actions
        self.batch_size = batch_size

        self._states = None
        self._q_s_a = None

        self._logits = None
        self._optimizer = None
        self._var_init = None

        self._define_model()

    def _define_model(self):
        self._states = tf.placeholder(shape=[None, self.num_states], dtype=tf.float32)
        self._q_s_a = tf.placeholder(shape=[None, self.num_actions], dtype=tf.float32)

        fully_connected_layer_1 = tf.layers.dense(self._states,
                                                  50, activation=tf.nn.relu)

        fully_connected_layer_2 = tf.layers.dense(fully_connected_layer_1,
                                                  50, activation=tf.nn.relu)

        self._logits = tf.layers.dense(fully_connected_layer_2,
                                       self.num_actions)

        loss = tf.losses.mean_squared_error(self._q_s_a, self._logits)
        self._optimizer = tf.train.AdamOptimizer().minimize(loss)
        self._var_init = tf.global_variables_initializer()

    def predict_one(self, state, sess):
        return sess.run(self._logits,
                        feed_dict={self._states: state.reshape(1, self.num_states)})

    def predict_batch(self, states, sess):
        return sess.run(self._logits,
                        feed_dict={self._states: states})

    def train_batch(self, sess, x_batch, y_batch):
        sess.run(self._optimizer,
                 feed_dict={self._states: x_batch, self._q_s_a: y_batch})
