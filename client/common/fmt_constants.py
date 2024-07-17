from socket import *
from struct import *
from fcntl import *
from time import *
from select import *
from random import *

import errno

SUCCESS = 0
FAILURE = 1

MAX_MSG_LEN  = 4098
MAX_BANK_LEN = 200
MAX_NAME_LEN = 50
MAX_PASS_LEN = 100

DEFAULT_BUFFER_LEN = 1
DEFAULT_PACKET_LEN = (28 + DEFAULT_BUFFER_LEN)

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
OP_EXIST      = 0xbeeb
OP_MSGINVAL   = 0xbcdf
OP_NOTFOUND   = 0xbcdc
OP_UNKNOWN    = 0xbadf
class Profile:
    def __init__(self) -> None:
        self.username   = ""
        self.password   = ""
        self.email      = ""
        self.profile_id = 0
        
    
def recv_full_data(client, expected_len):
    buffer = bytearray()
    recv_bytes = 0

    while (expected_len - recv_bytes) > 0:
        try:
            buffer.extend(client.recv((expected_len - recv_bytes)))
            recv_bytes = len(buffer)
        except error as e:
            err = e.args[0]
            if err == errno.EAGAIN or err == errno.EWOULDBLOCK:
                continue
            else:
                client.close()
                print(e)
                
                return

    return bytes(buffer)

def send_full_data(client, buffer, buffer_size):
    total_sent = 0

    while len(buffer):
        try:
            sent = client.send(buffer)
            total_sent += sent
            buffer = buffer[sent:]
        except error as e:
            if e.errno != errno.EAGAIN:
                raise e
            select([], [client], [])  # This blocks until

def send_data(client, bytes_to_send):
    offset = 0
    packet = b''
    seq_num = randint(0, 2000000)
    total_size = len(bytes_to_send)
    bytes_size = 0
    total_packets = 1 if (len(bytes_to_send) <= DEFAULT_BUFFER_LEN) else ((len(bytes_to_send) // DEFAULT_BUFFER_LEN) + 1)
    
    for i in range(0, total_packets):
        if total_packets == (i + 1):
            bytes_size = (total_size - offset)
            if 8 > DEFAULT_BUFFER_LEN:
                packet = pack("!iQQQ", seq_num, total_size, bytes_size, total_packets) + bytes_to_send[offset:].ljust(DEFAULT_BUFFER_LEN, b'\x00').ljust(8, b'\x00')
            else:
                packet = pack("!iQQQ", seq_num, total_size, bytes_size, total_packets) + bytes_to_send[offset:].ljust(DEFAULT_BUFFER_LEN, b'\x00')
            send_full_data(client, packet, len(packet))
        else:
            bytes_size = DEFAULT_BUFFER_LEN
            if 8 > DEFAULT_BUFFER_LEN:
                packet = pack("!iQQQ", seq_num, total_size, bytes_size, total_packets) + bytes_to_send[offset:(offset + (DEFAULT_BUFFER_LEN + 1))].ljust(8, b'\x00')
            else:
                packet = pack("!iQQQ", seq_num, total_size, bytes_size, total_packets) + bytes_to_send[offset:(offset + (DEFAULT_BUFFER_LEN + 1))]
            offset = (offset + DEFAULT_BUFFER_LEN)
            send_full_data(client, packet, len(packet))

def recieve_data(client):
    offset = 0
    data_to_return = b''
    seq_num = 0
    total_size = 0
    bytes_size = 0
    total_packets = 0

    temp_bytes = recv_full_data(client, DEFAULT_PACKET_LEN)

    packet = unpack("!iQQQ", temp_bytes[:(28 + 1)])

    seq_num = packet[0]
    total_size = packet[1]
    bytes_size = packet[2]
    total_packets = packet[3]
    
    data_to_return += temp_bytes[28:(bytes_size + 1)]
    
    offset += bytes_size

    for i in range(1, total_packets):
        temp_bytes = recv_full_data(client, DEFAULT_PACKET_LEN)

        packet = unpack("!iQQQ", temp_bytes[:(28 + 1)])

        seq_num = packet[0]
        total_size = packet[1]
        bytes_size = packet[2]
        total_packets = packet[3]
        
        data_to_return += temp_bytes[28:(bytes_size + 1)]
        
        offset += bytes_size

    if not (total_size == offset):
        print("\n\nERROR [x]  Failed to recieve full packets\n\n")