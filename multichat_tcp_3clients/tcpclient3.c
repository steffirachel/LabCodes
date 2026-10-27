#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main()
{
    int s;
    struct sockaddr_in server;
    char msg[100];

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    connect(s, (struct sockaddr *)&server, sizeof(server));

    printf("Client 3 - Enter message: ");
    fgets(msg, sizeof(msg), stdin);

    send(s, msg, strlen(msg), 0);

    close(s);
    return 0;
}
