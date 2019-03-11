import argparse
import gym_quake_2.envs.duel_env as q2

class Agent(object):
    def __init__(self, action_space):
        self.action_space = action_space

    def act(self):
        return self.action_space.sample()

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--socket", help="path to UNIX socket.", default="../../quake_socket")
    parser.add_argument("--path", help="path to where quake 2 is.", default="../../release")
    parser.add_argument("--episodes", help="number of episodes.", default="10")
    args = parser.parse_args()

    env = q2.DuelEnv(args.socket, args.path, 0, 0, True, 0, 0, 0)
    env.seed(123)
    agent = Agent(env._action_space())

    episode = 0
    while episode < int(args.episodes):
        obs = env._reset()
        done = False
        while not done:
            action = agent.act()
            obs, reward, done, info = env._step(action)
        episode += 1

    env.close()
