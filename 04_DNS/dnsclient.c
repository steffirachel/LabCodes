#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
void main() {
    int s, n, i, count = 0, found;
    char msg[100], ip[100];
struct timeval start,end;
double time,rtt,throughput;
    char cacheDomain[10][100], cacheIP[10][100];

    while (1) {
        printf("\nEnter domain (exit to stop): ");
        scanf("%99s", msg);

        if (strcmp(msg, "exit") == 0)
            break;

        found = 0;

        for (i = 0; i < count; i++) {
            if (strcmp(msg, cacheDomain[i]) == 0) {
                strcpy(ip, cacheIP[i]);
                found = 1;
                break;
            }
        }

        if (found) {
            printf("IP: %s (From Cache)\n", ip);
            continue;
        }

        s = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(8080);
        inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

        connect(s, (struct sockaddr*)&server, sizeof(server));
gettimeofday(&start,NULL);
        send(s, msg, strlen(msg)+1, 0);

        n = recv(s, ip, sizeof(ip)-1, 0);
        ip[n] = '\0';
gettimeofday(&end,NULL);
        printf("IP: %s\n", ip);

        if (strcmp(ip, "NXDOMAIN") != 0 && count < 10) {
            strcpy(cacheDomain[count], msg);
            strcpy(cacheIP[count], ip);
            count++;
        }

        close(s);
time=(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000000.0;
rtt=time*1000;
printf("%f and %f",time,rtt);
    }
}