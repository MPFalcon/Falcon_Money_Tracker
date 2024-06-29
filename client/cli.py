import argparse
from common.fmt_constants import *

MIN_PORT_RANGE  = 1024
MAX_PORT_RANGE  = 65535

parser = argparse.ArgumentParser(
    prog='Falcon Money Tracker GUI',
    description='Track all your balances in any bank you want!',
    epilog='Falcon Productions'
)

parser.add_argument(
    '-s',
    help="IP address of server",
    metavar="IP",
    action='store'
)

parser.add_argument(
    '-p',
    help="Port to connect to",
    metavar="PORT",
    action='store'
)

parser.add_argument_group

args = vars(parser.parse_args())

def check_port(port): 
    if MIN_PORT_RANGE > port or MAX_PORT_RANGE < port:
        print(f"\n\nPort must be between {MIN_PORT_RANGE} and {MAX_PORT_RANGE}\n\n")
        exit(FAILURE)

    return port

def handle_args(passed_args):
    if not args['s'] == None:
        passed_args[0] = args['s']
    if not args['p'] == None:
        try:
            passed_args[1] = check_port(int(args['p'], 10))
        except(ValueError):
            print("\n\nPort must be a valid integer\n\n")
            exit(FAILURE)

    return passed_args