import argparse
import gym_quake_2.envs.duel_env as q2
import gym_quake_2.envs.quake_2_env as base

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
    parser.add_argument("--seed", help="environment seed value.", \
                        default="0")

    args = parser.parse_args()
    env = None

    if args.host == "y":
        env = q2.DuelEnv(args.socket, args.path, \
                         args.address, None, \
                         args.level, True, \
                         args.timescale, \
                         args.render)
    else:
        env = q2.DuelEnv(args.socket, args.path, \
                         args.address, args.port, \
                         None, False, \
                         args.timescale, \
                         args.render)

    base.Runner(int(args.seed), args.episodes, env).run()
