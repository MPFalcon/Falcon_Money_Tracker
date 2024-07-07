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

def sign_up(profile, client, instructions):
    profile.username = input("\n> What's your new username: ")
    profile.password = input("\n> What's your new password: ")
    profile.email = input("\n> What's your email: ")

    data_len_sets = (len(profile.username), len(profile.password), len(profile.email))
    bytes_formats = ("!Q", "!Q", "!Q")

    send_full_data(client, instructions.pack_instructions(SIGNUP, calcsize("!QQQ")), INSTRUCTION_HDR_LEN)
    send_full_data(client, profile.packed_metadata(bytes_formats, data_len_sets), calcsize("!QQQ"))

    for i in (profile.username, profile.password, profile.email):
        send_full_data(client, i.encode("utf-8"), len(i))

    profile.profile_id = unpack("!Q", recv_full_data(client, calcsize("!Q")))[0]

    print(f"\n\nNew Profile ID: {profile.profile_id}\n\n")

    ret_code = get_code(recv_full_data(client, 2))

    process_return_code(ret_code)

def menu(client, instructions):
    profile = Profile()

    print('''
            1) Login
            2) Sign Up
            3) Exit
            4) Help
            ''')
    
    while True:
      
      user_input = input("\n>  ")

      if user_input == '1':
          send_full_data(client, instructions.pack_instructions(LOGIN, 0), INSTRUCTION_HDR_LEN)
      if user_input == '2':
          sign_up(profile, client, instructions)
      if user_input == '3':
          send_full_data(client, instructions.pack_instructions(TERMINATE_SESSION, 0), INSTRUCTION_HDR_LEN)
          
          break
      if user_input == '4':
          print('''
            1) Login
            2) Sign Up
            3) Exit
            4) Help
            ''')