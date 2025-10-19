from enum import IntEnum
import socket

HOST = "0.0.0.0"
PORT = 43215
ADDR = (HOST, PORT)
socket_connection = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
socket_connection.settimeout(1.0)

class MessageType(IntEnum):
    GET_SERVER_INFO = 13
    REDIRECT_PLAYER = 90
    REDIRECT_ALL = 91

def send(*args, response=False):
    payload = bytes(args)
    socket_connection.sendto(payload, ADDR)

    if response:
        return socket_connection.recvfrom(1024)

def get_server_info(server_id):
    server_info = send(MessageType.GET_SERVER_INFO, server_id)
    return server_info
