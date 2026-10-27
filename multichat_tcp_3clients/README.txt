MULTI-CLIENT TCP SERVER USING PTHREAD

FILES
-----
multichatserver.c - Server using pthread; accepts multiple clients.
tcpclient1.c      - Client 1
tcpclient2.c      - Client 2
tcpclient3.c      - Client 3
README.txt        - These instructions

WHAT THE CODE DOES
------------------
The server listens on TCP port 8080. Each client connects and sends one
message. The server handles each connection in a separate thread and prints
the message. The server can accept up to 100 connections in this example.

This is a simple learning example, not a full chat room: messages are printed
on the server and are not broadcast to other clients.

HOW TO RUN (LINUX / WSL)
------------------------
1. Extract the ZIP and open a terminal in the extracted folder.

2. TERMINAL 1 - compile and start the server:
   gcc multichatserver.c -o multichatserver -pthread
   ./multichatserver

   Keep this terminal running.

3. TERMINAL 2 - compile and run client 1:
   gcc tcpclient1.c -o tcpclient1
   ./tcpclient1

4. TERMINAL 3 - compile and run client 2:
   gcc tcpclient2.c -o tcpclient2
   ./tcpclient2

5. TERMINAL 4 - compile and run client 3:
   gcc tcpclient3.c -o tcpclient3
   ./tcpclient3

6. Enter a different message in each client terminal and press Enter.
   The server terminal displays the messages.

NOTES
-----
- The clients connect to 127.0.0.1, which means the same computer.
- Stop the server with Ctrl+C.
- If port 8080 is busy, stop the other program or change the port in the
  server and all three client files.
- Install gcc on Ubuntu/WSL if needed:
      sudo apt update
      sudo apt install build-essential
