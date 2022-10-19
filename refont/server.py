import socket
import threading


HEADER = 64
PORT = 5050
#SERVER = "192.168.1.11"
SERVER = socket.gethostbyname(socket.gethostname())
ADDR = (SERVER, PORT)
FORMAT = "utf-8"
DISCONNECT_MESSAGE = "!DISCONNECT"

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(ADDR)

def handle_client(conn, addr):
    print(f"[NEW CONNECTION  {addr} connected]")

    connected = True
    while connected:
        msg_header = conn.recv(HEADER).decode(FORMAT)
        if msg_header:
            msg_length = int(msg_header)
            msg = conn.recv(msg_length).decode(FORMAT)

            if (msg == DISCONNECT_MESSAGE):
                connected = False

            print(f"[{addr}] {msg}")
            conn.send("msg received".encode(FORMAT))
    
    conn.close()


def start():
    server.listen()
    print(f"[listening] Server is listening on {SERVER}")
    while True:
        conn, addr = server.accept()
        thread = threading.Thread(target=handle_client, args=(conn, addr))
        thread.start()
        print(f"[ACTIVE CONNECTION] : {threading.active_count() - 1}")

if __name__ == "__main__":

    print("[STARTING] server is starting...")
    start()

