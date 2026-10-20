#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

int main()
{
    int s, n, i, count = 0;
    char domain[100], ip[100];
    char cacheDomain[10][100], cacheIP[10][100];
    struct sockaddr_in server = {0};

    while (1) {
        printf("Enter domain (or exit): ");
        if (fgets(domain, sizeof(domain), stdin) == NULL)
            break;
        domain[strcspn(domain, "\n")] = '\0';
        if (strcmp(domain, "exit") == 0)
            break;

        for (i = 0; i < count; i++) {
            if (strcmp(domain, cacheDomain[i]) == 0) {
                printf("Cached IP address: %s\n", cacheIP[i]);
                break;
            }
        }
        if (i < count)
            continue;

        s = socket(AF_INET, SOCK_STREAM, 0);
        struct timeval timeout;
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        server.sin_family = AF_INET;
        server.sin_port = htons(8080);
        server.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
            printf("Could not connect to DNS server\n");
            close(s);
            continue;
        }
        send(s, domain, strlen(domain), 0);

        n = recv(s, ip, sizeof(ip) - 1, 0);
        if (n > 0) {
            ip[n] = '\0';
            printf("IP address: %s\n", ip);
            if (strcmp(ip, "NXDOMAIN") != 0 && count < 10) {
                strcpy(cacheDomain[count], domain);
                strcpy(cacheIP[count], ip);
                count++;
            }
        } else {
            printf("DNS request timed out\n");
        }
        close(s);
    }
    return 0;
}
