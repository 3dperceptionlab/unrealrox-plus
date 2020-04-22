import socket

server = "127.0.0.1"
port = 8890

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect((server, port))
client.settimeout(2)

while True:
    message = input("Command: ")
    if message == "exit":
        break
    client.send(message.encode())
    try:
        response = client.recv(4096)
        print(response.decode())
    except:
        None

client.close()

