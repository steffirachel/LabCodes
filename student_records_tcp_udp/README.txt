MULTI-CLIENT TCP AND UDP STUDENT RECORD APPLICATION

Requirements:
Linux or WSL with GCC and pthread support.

Keep students.csv in the same directory as the server programs.

Compile the TCP server:
gcc tcpserver.c -o tcpserver -pthread

Compile the TCP client:
gcc tcpclient.c -o tcpclient

Compile the UDP server:
gcc udpserver.c -o udpserver

Compile the UDP client:
gcc udpclient.c -o udpclient

Run TCP:
Terminal 1:
./tcpserver

Terminal 2:
./tcpclient

Open more terminals and run ./tcpclient to test multiple TCP clients.

Run UDP:
Terminal 1:
./udpserver

Terminal 2:
./udpclient

Example valid roll number:
101

Expected TCP response:
101,Arun,CSE,3,8.5

Expected UDP response:
101,Arun

Example invalid roll number:
999

Expected response:
Student Record Not Found

Important:
Run the servers from the directory containing students.csv.
TCP uses port 8080.
UDP uses port 8081.
