import os
from cli import *

def get_code(byte_code):
    return unpack_from("!H", byte_code)[0]

def sign_up(profile, client, instructions):
    profile.username = input("\n> What's your new username: ")
    profile.password = input("\n> What's your new password: ")

    data_len_sets = (len(profile.username), len(profile.password))
    bytes_formats = ("!Q", "!Q")

    send_full_data(client, instructions.pack_instructions(SIGNUP, calcsize("!QQ")), INSTRUCTION_HDR_LEN)
    send_full_data(client, profile.packed_metadata(bytes_formats, data_len_sets), calcsize("!QQ"))

    for i in (profile.username, profile.password):
        send_full_data(client, i.encode("utf-8"), len(i))

    profile.profile_id = unpack("!Q", recv_full_data(client, calcsize("!Q")))

    print("\n\n")

    if OP_SUCCESS == get_code(recv_full_data(client, 2)):
        print("\n\nSuccess!\n")
        sleep(5.0)

def menu(client, instructions):
    profile = Profile()

    while True:
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