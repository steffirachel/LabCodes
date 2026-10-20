#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int s, c, n;
    char msg[100];

    s = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&server, sizeof(server));
    listen(s, 1);

    printf("Waiting for client...\n");
    c = accept(s, NULL, NULL);

    n = recv(c, msg, sizeof(msg) - 1, 0);
    if (n > 0) {
        msg[n] = '\0';
        printf("Client says: %s\n", msg);
        send(c, "Hello Client", 12, 0);
    }

    close(c);
    close(s);
    return 0;
}
