import os
from cli import *

def menu(client, instructions):
    while True:
      os.system("clear")
      print('''
          1) Login
          2) Sign Up
          3) Exit
            ''')
      user_input = input("\n>  ")

      if user_input == '1':
          client.send(instructions.pack_instructions(LOGIN, 0))
      if user_input == '2':
          client.send(instructions.pack_instructions(SIGNUP, 0))
      if user_input == '3':
          client.send(instructions.pack_instructions(TERMINATE_SESSION, 0))

          break