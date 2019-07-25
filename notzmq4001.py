#!/usr/bin/env python3

import socket

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 4001        # The port used by the server

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    for request in range(10):
        print("Sending request %s ..." % request)
        # socket.send("GET / HTTP/1.0\r\n\r\n".encode("utf-8"))
        s.send(b'From port 4001.\r')
        # time.sleep(5);
        #  Get the reply.
        data = s.recv(1024)
        print('Received', repr(data))
