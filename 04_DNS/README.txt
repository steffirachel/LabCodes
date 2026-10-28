04_DNS — DNS Client and Server

WHAT THE CODE DOES
------------------
This program demonstrates a simple DNS-like lookup using TCP sockets on localhost.

dnsserver.c:
- Listens for a client connection on TCP port 8080.
- Receives a domain name from the client.
- Checks the domain against a fixed list:
  - google.com  -> 142.250.183.14
  - yahoo.com   -> 98.137.11.163
  - amazon.com  -> 205.251.242.103
- Sends the matching IP address back to the client.
- Sends "NXDOMAIN" if the domain is not in its list.
- Handles one client connection, then closes.

dnsclient.c:
- Repeatedly asks the user to enter a domain name.
- Checks its local cache first.
- If the domain is cached, displays the cached IP without contacting the server.
- Otherwise, connects to the server at 127.0.0.1:8080 and sends the domain.
- Displays the reply and stores successful lookups in a cache of up to 10 entries.
- Measures the time around sending the request and receiving the reply, then prints
  the time in seconds and milliseconds.
- Enter "exit" to stop the client.

Note: This is a small educational DNS simulation. It does not query real DNS servers
or perform a real Internet DNS lookup. The IP addresses are fixed values in the code.

HOW TO RUN (Linux / Ubuntu / WSL)
---------------------------------
1. Open a terminal and go to the folder containing the extracted files:
   cd 04_DNS

2. Compile the server:
   gcc dnsserver.c -o dnsserver

3. Compile the client:
   gcc dnsclient.c -o dnsclient

4. Open Terminal 1 and start the server:
   ./dnsserver

5. Open Terminal 2, go to the same folder, and start the client:
   ./dnsclient

6. Try these domain names:
   google.com
   yahoo.com
   amazon.com

7. Try an unknown domain, for example:
   example.com
   The server should reply with NXDOMAIN.

8. Enter the same known domain again to see the "(From Cache)" message.
   Enter exit to stop the client.

IMPORTANT RUNNING NOTE
----------------------
The server code accepts only one client connection and then exits. Therefore, after
the first uncached lookup, the server will stop. To test another uncached domain,
start ./dnsserver again in Terminal 1 before entering that domain in the client.
Cached lookups do not need a server connection.

INFERENCE
---------
- A client can request an IP address for a domain from a server using TCP sockets.
- The server returns an IP address only for the three domain names in its fixed list.
- Unknown domain names receive the response "NXDOMAIN".
- Client-side caching avoids sending a new request for a domain already looked up
  successfully, reducing repeated network communication.
- The program measures and prints the request/reply elapsed time in seconds and
  milliseconds for uncached lookups.
- This is a basic demonstration of name resolution, caching, and client-server
  communication, not a complete implementation of DNS.

The C source files are included exactly as provided; no code changes were made.
