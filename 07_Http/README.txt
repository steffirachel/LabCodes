ASSIGNMENT 3: IMPLEMENTATION OF HTTP CLIENT USING TCP SOCKETS
================================================================

FILES INCLUDED
--------------
httpclient.c  - C TCP socket client. Requests a resource from the local
                Python HTTP server, saves the response body using the
                requested file's name, and prints performance measurements.
index.html    - Sample HTML page hosted by the Python server.
notes.txt     - Sample text file hosted by the Python server.

REQUIREMENTS
------------
- Linux, Ubuntu, or WSL
- GCC compiler
- Python 3

HOW TO RUN
----------

STEP 1: Open a terminal and enter the folder
--------------------------------------------
After extracting the ZIP, enter the extracted folder. For example:

    cd CN_Web_Server

STEP 2: Start the Python web server
-----------------------------------
In Terminal 1, run:

    python3 -m http.server 8000

Expected output:

    Serving HTTP on 0.0.0.0 port 8000

Keep this terminal open.

STEP 3: Compile the C client
----------------------------
Open Terminal 2, go to the same folder, and run:

    gcc httpclient.c -o httpclient

STEP 4: Run the client
----------------------
In Terminal 2, run:

    ./httpclient

Enter this URL:

    http://localhost:8000/index.html

The downloaded response body is saved as index.html in the directory
where ./httpclient is running.

To download the text file, run ./httpclient again and enter:

    http://localhost:8000/notes.txt

It is saved as notes.txt.

STEP 5: Check the file
----------------------
For example:

    cat notes.txt

IMPORTANT NOTES
---------------
- This client is intended for the local Python server on 127.0.0.1.
- Enter a URL containing a port, such as http://localhost:8000/index.html.
- The file is saved with the name at the end of the URL.
- If the URL ends in '/', the client uses index.html as the filename.
- If a file with the same name already exists in the current directory,
  it will be overwritten. To protect the sample files, run the client
  from a separate download directory or back up files first.
- This educational client supports HTTP, not HTTPS.
- Data size is measured in bytes and throughput in bytes per second.
- Timing values depend on your system and may be very small.

SAMPLE OUTPUT
-------------
Enter URL: http://localhost:8000/index.html
HTTP Response: HTTP/1.0 200 OK
Response time: (varies) seconds
Download time: (varies) seconds
Data size: (varies) bytes
Throughput: (varies) bytes/second
Saved as index.html

INFERENCE
---------
The C client uses TCP sockets to send an HTTP GET request to a Python
web server. The server returns the requested resource, and the client
saves the response body locally using the requested filename. The client
also measures response time, download time, data size, and throughput.
This demonstrates basic HTTP communication over TCP and performance
measurement.
