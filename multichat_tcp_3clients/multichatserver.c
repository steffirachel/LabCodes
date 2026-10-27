#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

int clients[100];
int idx[100];

void *client(void *arg)
{
    int i = *(int *)arg;
    int c = clients[i], n;
    char msg[100];

    n = recv(c, msg, sizeof(msg) - 1, 0);

    if (n > 0)
    {
        msg[n] = '\0';
        printf("Client says: %s\\n", msg);
    }

    close(c);
    return NULL;
}

int main()
{
    int s, c, i = 0;
    pthread_t t[100];
    struct sockaddr_in server;

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&server, sizeof(server));
    listen(s, 5);

    printf("Waiting for clients...\\n");

    while (i < 100)
    {
        c = accept(s, NULL, NULL);

        clients[i] = c;
        idx[i] = i;

        pthread_create(&t[i], NULL, client, &idx[i]);
        pthread_detach(t[i]);

        i++;
    }

    close(s);
    return 0;
}
