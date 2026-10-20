#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

int main()
{
    int s, n;
    char url[300], path[200], host[100], request[500], response[4096];
    struct sockaddr_in server = {0};
    FILE *fp;
    struct timeval start, end;

    printf("Enter URL (example http://localhost:8000/notes.txt): ");
    scanf("%299s", url);

    if (strncmp(url, "http://", 7) != 0) {
        printf("Use an http:// URL\n");
        return 1;
    }

    char *p = url + 7;
    char *slash = strchr(p, '/');
    if (slash == NULL) {
        printf("URL must include a file path\n");
        return 1;
    }

    int hostlen = slash - p;
    if (hostlen >= (int)sizeof(host)) return 1;
    strncpy(host, p, hostlen);
    host[hostlen] = '\0';
    strcpy(path, slash);

    char *colon = strchr(host, ':');
    int port = 80;
    if (colon != NULL) {
        *colon = '\0';
        port = atoi(colon + 1);
    }

    s = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    gettimeofday(&start, NULL);
    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        close(s);
        return 1;
    }

    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
             path, host);
    send(s, request, strlen(request), 0);

    fp = fopen("downloaded_file", "wb");
    if (fp == NULL) {
        perror("fopen");
        close(s);
        return 1;
    }

    int header_done = 0;
    char all[200000];
    int total = 0;
    while ((n = recv(s, response, sizeof(response) - 1, 0)) > 0) {
        if (!header_done) {
            response[n] = '\0';
            char *body = strstr(response, "\r\n\r\n");
            if (body != NULL) {
                body += 4;
                fwrite(body, 1, n - (body - response), fp);
                total += n - (body - response);
                header_done = 1;
            }
        } else {
            fwrite(response, 1, n, fp);
            total += n;
        }
    }
    fclose(fp);
    gettimeofday(&end, NULL);

    double seconds = (end.tv_sec - start.tv_sec) +
                     (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Downloaded to downloaded_file\n");
    printf("Data size: %d bytes\n", total);
    printf("Download time: %.3f seconds\n", seconds);
    if (seconds > 0)
        printf("Throughput: %.2f bytes/second\n", total / seconds);

    close(s);
    return 0;
}
