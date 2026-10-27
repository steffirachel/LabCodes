#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main()
{
    int s;
    char roll[100];
    char msg[300];
    struct sockaddr_in server;
    socklen_t len = sizeof(server);

    s = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8081);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter Roll Number: ");
    scanf("%99s", roll);

    sendto(s, roll, strlen(roll), 0,
           (struct sockaddr *)&server, len);

    int n = recvfrom(s, msg, sizeof(msg) - 1, 0, NULL, NULL);

    if (n > 0) {
        msg[n] = '\0';
        printf("Server Response: %s\n", msg);
    }

    close(s);
    return 0;
}
