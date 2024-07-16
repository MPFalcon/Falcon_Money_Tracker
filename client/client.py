import fcntl
from menu import *

def send_token(client):
    token_bytes = pack("!Q", AUTH_CLIENT)
    send_data(client, token_bytes)

def connect_to_server(server_info):
    client = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)
    client.connect((server_info[0], server_info[1]))
    fcntl(client, F_SETFD, os.O_NONBLOCK)

    return client

def main():
    server_info = ["127.0.0.1", 31337]
    server_info = handle_args(server_info)

    try:
        client_fd = connect_to_server(server_info)
        send_token(client_fd)
        menu(client_fd)
        client_fd.close()
    except(KeyboardInterrupt):
        print("\n\nNOTE [x]  Shutting Down...\n")
        client_fd.close()
        exit(SUCCESS)
    except(ConnectionResetError):
        print("\n\nNOTE [x]  Connection reset\n")
        client_fd.close()
        exit(SUCCESS)
    



main()