import os
from cli import *

def get_code(byte_code):
    return unpack_from("!H", byte_code)[0]

def sign_up(profile, client, instructions):
    username = input("\n> What's your new username: ")
    password = input("\n> What's your new password: ")

    profile.update_profile(username, password)

    send_full_data(client, instructions.pack_instructions(SIGNUP, len(profile.get_info_string())), INSTRUCTION_HDR_LEN)
    send_full_data(client, profile.get_info_string().encode("utf-8"), len(profile.get_info_string().encode("utf-8")))

    if OP_SUCCESS == get_code(recv_full_data(client, 2)):
        print("\n\nSuccess!\n")
        sleep(5.0)

def menu(client, instructions):
    profile = Profile()

    while True:
      os.system("clear")
      print('''
          1) Login
          2) Sign Up
          3) Exit
            ''')
      user_input = input("\n>  ")

      if user_input == '1':
          send_full_data(client, instructions.pack_instructions(LOGIN, 0), INSTRUCTION_HDR_LEN)
      if user_input == '2':
          sign_up(profile, client, instructions)
      if user_input == '3':
          send_full_data(client, instructions.pack_instructions(TERMINATE_SESSION, 0), INSTRUCTION_HDR_LEN)

          break