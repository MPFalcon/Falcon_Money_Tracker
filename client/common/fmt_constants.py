from socket import *
from struct import *

SUCCESS = 0
FAILURE = 1

MAX_MSG_LEN  = 4098
MAX_BANK_LEN = 200
MAX_NAME_LEN = 50
MAX_PASS_LEN = 100

AUTH_CLIENT       = 0xfeb4593fecc67839

RECV_READY        = 0xa2df
SEND_READY        = 0xa2ab
LOGIN             = 0xacdf
SIGNUP            = 0xacdc
ADD_BANK          = 0xaadf
ADD_BALANCE       = 0xa394
REMOVE_BANK       = 0xac91
REMOVE_BALANCE    = 0xaf43
UPDATE_BANK       = 0xa582
UPDATE_BALANCE    = 0xa239
TERMINATE_SESSION = 0xaffe
ERR_CODE          = 0xabfd

OP_SUCCESS    = 0xb2df
OP_ERR        = 0xb2ab
OP_MSGINVAL   = 0xbcdf
OP_NOTFOUND   = 0xbcdc
OP_UNKNOWN    = 0xbadf
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