import socket
import sys

HEADER = 64
PORT = 18002
FORMAT = "utf-8"
SERVER = "127.0.1.1"

post_req = '''POST /test/kind/hihi.json HTTP/1.1
Accept: application/json
Content-Type: application/json
Content-Length: 85
Host: localhost:18002

{
"Id": 00000,
"Customer": "John Smith",
"Quantity": 1,
"Price": 10.00
}

'''

del_req = '''DELETE /test/kind/hoho HTTP/1.1
Host: localhost:18002

'''

def send(req):

    if (req == "post"):
        msg = post_req
    elif (req == "del"):
        msg = del_req
    else:
        return
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

    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    addr = (SERVER, port)
    client.connect(addr)

    print("exit to quit")
    while True:
        msg = input()
        if msg == "exit":
            client.close()
            break

        send(msg)

