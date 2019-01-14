import argparse
import gym_quake_2.envs.duel_env as q2

class Agent(object):
    def __init__(self, action_space):
        self.action_space = action_space

    def act(self):
        return self.action_space.sample()

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--socket", help="path to UNIX socket.", default="")
    parser.add_argument("--episodes", help="number of episodes to run.", default="")
    args = parser.parse_args()

    env = q2.DuelEnv(args.socket, args.path)
    env.seed(123)
    agent = Agent(env.action_space)

    episode = 0
    while episode < args.episodes:
        obs = env.reset()
        done = False
        while not done:
            action = agent.act()
            obs, reward, done, info = env.step(action)
        episode += 1

    env.close()
