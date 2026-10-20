#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int s, n;
    char msg[100];

    s = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(s, (struct sockaddr *)&server, sizeof(server));

    printf("Enter message: ");
    fgets(msg, sizeof(msg), stdin);
    send(s, msg, strlen(msg), 0);

    n = recv(s, msg, sizeof(msg) - 1, 0);
    if (n > 0) {
        msg[n] = '\0';
        printf("Server says: %s\n", msg);
    }

    close(s);
    return 0;
}
