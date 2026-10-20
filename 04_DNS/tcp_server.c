#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int s, c, n;
    char domain[100], ip[100];
    struct sockaddr_in server = {0};

    s = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = INADDR_ANY;
    bind(s, (struct sockaddr *)&server, sizeof(server));
    listen(s, 5);

    printf("DNS server waiting...\n");
    while (1) {
        c = accept(s, NULL, NULL);
        n = recv(c, domain, sizeof(domain) - 1, 0);
        if (n > 0) {
            domain[n] = '\0';
            domain[strcspn(domain, "\n")] = '\0';

            if (strcmp(domain, "google.com") == 0)
                strcpy(ip, "142.250.183.14");
            else if (strcmp(domain, "yahoo.com") == 0)
                strcpy(ip, "98.137.11.163");
            else if (strcmp(domain, "timeout.test") == 0) {
                sleep(5);
                strcpy(ip, "10.0.0.5");
            } else
                strcpy(ip, "NXDOMAIN");

            send(c, ip, strlen(ip), 0);
        }
        close(c);
    }
    close(s);
    return 0;
}
