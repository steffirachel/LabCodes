#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void main() {
    int s, c, n, i;
    char msg[100], ip[100];

    char *domain[] = {"google.com", "yahoo.com", "amazon.com"};
    char *address[] = {"142.250.183.14", "98.137.11.163",
                       "205.251.242.103"};

    s = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr*)&server, sizeof(server));
    listen(s, 1);

    printf("Waiting for client...\n");
    c = accept(s, NULL, NULL);

    n = recv(c, msg, sizeof(msg)-1, 0);
    msg[n] = '\0';

    strcpy(ip, "NXDOMAIN");

    for (i = 0; i < 3; i++) {
        if (strcmp(msg, domain[i]) == 0) {
            strcpy(ip, address[i]);
            break;
        }
    }

    send(c, ip, strlen(ip)+1, 0);

    close(c);
    close(s);
}