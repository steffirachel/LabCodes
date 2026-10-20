# Simple Computer Networks Lab Codes (Ubuntu)

These are short beginner-style C socket programs, organized by lab exercise.
Run them inside Ubuntu or WSL, not in Windows PowerShell.

## 1. Install compiler
```bash
sudo apt update
sudo apt install build-essential
```

Go to the folder after extracting:
```bash
cd Networking_Lab_Codes
```

## 2. Normal TCP
Terminal 1:
```bash
cd 01_TCP
gcc server.c -o server
./server
```
Terminal 2, in the same folder:
```bash
gcc client.c -o client
./client
```
Enter a message in the client.

## 3. Normal UDP
Use a different terminal and stop any other program using port 8080 first.
Terminal 1:
```bash
cd 02_UDP
gcc server.c -o server
./server
```
Terminal 2:
```bash
cd 02_UDP
gcc client.c -o client
./client
```

## 4. Multi-client CSV (TCP)
Keep `students.csv` in the same folder as the server.
Terminal 1:
```bash
cd 03_MultiClient_CSV
gcc tcp_server.c -o tcp_server -pthread
./tcp_server
```
Terminal 2:
```bash
cd 03_MultiClient_CSV
gcc tcp_client.c -o tcp_client
./tcp_client
```
Try roll numbers `101`, `102`, or `103`. Run multiple clients in separate terminals to test concurrent connections.

## 5. CSV lookup (UDP)
Terminal 1:
```bash
cd 03_MultiClient_CSV
gcc udp_server.c -o udp_server
./udp_server
```
Terminal 2:
```bash
cd 03_MultiClient_CSV
gcc udp_client.c -o udp_client
./udp_client
```
This uses UDP port 8081.

## 6. DNS with timeout
Terminal 1:
```bash
cd 04_DNS
gcc tcp_server.c -o server
./server
```
Terminal 2:
```bash
cd 04_DNS
gcc tcp_client.c -o client
./client
```
Try `google.com`, `yahoo.com`, an unknown name, or `timeout.test`.
`timeout.test` waits five seconds on the server, while the client times out after three seconds. The client has a simple in-memory cache for successful lookups during that one run.
This is a small local DNS simulation, not a real internet DNS client.

## 7. Checksum
Terminal 1:
```bash
cd 05_Checksum
gcc server.c -o server
./server
```
Terminal 2:
```bash
cd 05_Checksum
gcc client.c -o client
./client
```
Enter two numbers between 0 and 255. The client calculates an 8-bit one's-complement checksum and sends the two numbers plus checksum; the server checks whether the final folded sum is 255.

## 8. CRC
Terminal 1:
```bash
cd 06_CRC
gcc crc_server.c -o server
./server
```
Terminal 2:
```bash
cd 06_CRC
gcc crc_client.c -o client
./client
```
Enter one character, then choose `0` for no error or `1` to flip the first bit. Generator: `10011`. The client converts the character to 8-bit ASCII, adds the CRC remainder, and sends the frame. The server divides it and checks the remainder.

## 9. HTTP request to Python web server
Terminal 1:
```bash
cd 07_HTTP
python3 -m http.server 8000
```
Terminal 2:
```bash
cd 07_HTTP
gcc client.c -o client
./client
```
Enter:
```text
http://localhost:8000/notes.txt
```
The response body is saved as `downloaded_file` in the current directory.
Try `http://localhost:8000/index.html`, `http://localhost:8000/sample.pdf`, or `http://localhost:8000/image.jpg` too.
The sample client is for a local Python server using `localhost`; it does not support HTTPS.

## Notes
- For TCP and UDP, server and client must use matching ports.
- Run the server before the client.
- Press `Ctrl+C` to stop a server that is waiting for connections.
- TCP and UDP normal examples both use port 8080, so do not run both servers at the same time.
- The CSV examples use `students.csv` from their own folder.
