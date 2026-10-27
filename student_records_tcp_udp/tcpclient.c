#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main()
{
    int s;
    char roll[100];
    char msg[1000];
    struct sockaddr_in server;

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(s, (struct sockaddr *)&server, sizeof(server));

    printf("Enter Roll Number: ");
    scanf("%99s", roll);

    send(s, roll, strlen(roll), 0);

    int n = recv(s, msg, sizeof(msg) - 1, 0);
    if (n > 0) {
        msg[n] = '\0';
        printf("Server Response: %s\n", msg);
    }

    close(s);
    return 0;
}
