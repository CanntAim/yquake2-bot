import argparse
import gym_quake_2.envs.duel_env as q2

class Agent(object):
    def __init__(self, action_space):
        self.action_space = action_space

    def act(self):
        return self.action_space.sample()

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--socket", help="path to UNIX socket.", \
                        default="../../quake_socket")
    parser.add_argument("--path", help="path to where quake 2 is.", \
                        default="../../release")
    parser.add_argument("--host", help="whether this client is host or not.", \
                        default="y")
    parser.add_argument("--address", help="address of server to connect to.", \
                        default="localhost")
    parser.add_argument("--port", help="port of server to connect to.", \
                        default="")
    parser.add_argument("--level", help="map to train on.", \
                        default="bottrain")
    parser.add_argument("--episodes", help="number of episodes.", \
                        default="10")
    parser.add_argument("--timescale", help="game time scale.", \
                        default="1")
    parser.add_argument("--render", help="whether to run headless.", \
                        default="n")
    args = parser.parse_args()
    env = None
    if args.host == "y":
        env = q2.DuelEnv(args.socket, args.path, \
                         args.address, None, \
                         args.level, True,
                         args.timescale, args.render)
    else:
        env = q2.DuelEnv(args.socket, args.path, \
                         args.address, args.port, \
                         None, False, \
                         args.timescale, args.render)
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
