import socket
import sys

HEADER = 64
PORT = 18000
FORMAT = "utf-8"
SERVER = "127.0.1.1"

def send(msg):
    message = msg.encode(FORMAT)
#    msg_length = len(message)
#    send_length = str(msg_length).encode(FORMAT)
#    send_length += b' ' * (HEADER - len(send_length))
#    client.send(send_length)
    client.send(message)
    print()
    print(client.recv(2048).decode(FORMAT))


if __name__ == "__main__":

    if (len(sys.argv) > 1):
        arg = str(sys.argv[1])
        try:
            port = int(sys.argv[1])
        except:
            port = PORT
    else:
        port = PORT

    print(f"[PORT] = {port}\n")

    client = socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    addr = (SERVER, port)
    client.connect(addr)

    print("exit to quit")
    while True:
        msg = input()
        if msg == "exit":
            break

        send(msg)

