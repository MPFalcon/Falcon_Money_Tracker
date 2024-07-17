import os
from cli import *

def process_return_code(ret_code):
    if OP_SUCCESS == ret_code:
        print("\n\nSuccess!\n")
        sleep(5.0)
    elif OP_ERR == ret_code:
        print("\n\nServer unable to process informtion provided\n\n")
        sleep(5.0)
    elif OP_EXIST == ret_code:
        print("\n\nInformation already exists in database\n\n")
        sleep(5.0)
    elif OP_MSGINVAL == ret_code:
        print("\n\nServer unable to process informtion provided: Invalid message\n\n")
        sleep(5.0)
    elif OP_UNKNOWN == ret_code:
        print("\n\nAn unknown error has occurred during this operation\n\n")
        sleep(5.0)

def get_code(byte_code):
    return unpack_from("!H", byte_code)[0]

def sign_up(profile, client):
    profile.username = input("\n> What's your new username: ")
    profile.password = input("\n> What's your new password: ")
    profile.email = input("\n> What's your email: ")

    for i in (profile.username, profile.password, profile.email):
        send_data(client, i.encode("utf-8"))

    ret_code = get_code(recieve_data(client))
    process_return_code(ret_code)
    
def menu(client):
    profile = Profile()

    print('''
            1) Login
            2) Sign Up
            3) Exit
            4) Help
            ''')
    
    while True:
      
      user_input = input("\n>  ")

      #if user_input == '1':
          
      if user_input == '2':
          send_data(client, pack("!H", SIGNUP))
          #sign_up(profile, client)
      if user_input == '3':
          break
      if user_input == '4':
          print('''
            1) Login
            2) Sign Up
            3) Exit
            4) Help
            ''')