from socket import *
from struct import *

SUCCESS = 0
FAILURE = 1

MAX_MSG_LEN  = 4098
MAX_BANK_LEN = 200
MAX_NAME_LEN = 50
MAX_PASS_LEN = 100

AUTH_CLIENT       = 0xfeb4593fecc67839
TERMINATE_SESSION = 0xfffe

RECV_READY     = 0x12df
SEND_READY     = 0x12ab
LOGIN          = 0xccdf
SIGNUP         = 0xccdc
ADD_BANK       = 0xdadf
ADD_BALANCE    = 0x2394
REMOVE_BANK    = 0xcc91
REMOVE_BALANCE = 0xff43
UPDATE_BANK    = 0x6582
UPDATE_BALANCE = 0x2239

class Instruction_Header:
    def __init__(self) -> None:
        self.op_code   = 0x0
        self.byte_size = 0
    
    def pack_instructions(self, op_code, byte_size):
        self.op_code   = op_code
        self.byte_size = byte_size

        return pack("!HQ", self.op_code, self.byte_size)
    
    def unpack_instructions(bytes):
        return unpack_from("!HQ", bytes)

    def show_instructions(self):
        print(f"({self.op_code}, {self.byte_size})")