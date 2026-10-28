#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

int main() {
    int s, n;
    char url[300], host[100], path[200];
    char request[500], buffer[4096];
    char *data = NULL, *body, *p;
    int size = 0, header = 0;
    struct sockaddr_in server;
    struct timeval start, first, end;
    double response, download, throughput;
    FILE *fp;

    printf("Enter URL: ");
    scanf("%299s", url);

    if (strncmp(url, "http://", 7) != 0) {
        printf("Enter an HTTP URL\n");
        return 1;
    }

    p = url + 7;
    sscanf(p, "%99[^:/]", host);

    p = strchr(p, ':');
    if (p == NULL) {
        printf("Use URL with port 8000\n");
        return 1;
    }

    p++;
    int port = atoi(p);

    p = strchr(p, '/');
    if (p == NULL)
        strcpy(path, "/");
    else
        strcpy(path, p);

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connection failed");
        close(s);
        return 1;
    }

    sprintf(request, "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", path, host);

    gettimeofday(&start, NULL);
    send(s, request, strlen(request), 0);

    while ((n = recv(s, buffer, sizeof(buffer), 0)) > 0) {
        if (data == NULL)
            gettimeofday(&first, NULL);

        char *temp = realloc(data, size + n);
        if (temp == NULL) {
            printf("Memory allocation failed\n");
            free(data);
            close(s);
            return 1;
        }
        data = temp;
        memcpy(data + size, buffer, n);
        size += n;
    }

    gettimeofday(&end, NULL);
    close(s);

    if (data == NULL) {
        printf("No response received\n");
        return 1;
    }

    response = (first.tv_sec - start.tv_sec) +
        (first.tv_usec - start.tv_usec) / 1000000.0;
    download = (end.tv_sec - start.tv_sec) +
        (end.tv_usec - start.tv_usec) / 1000000.0;

    body = NULL;
    for (int i = 0; i < size - 3; i++) {
        if (memcmp(data + i, "\r\n\r\n", 4) == 0) {
            body = data + i + 4;
            header = i + 4;
            break;
        }
    }

    if (body == NULL) {
        printf("Invalid HTTP response\n");
        free(data);
        return 1;
    }

    char filename[200];
    p = strrchr(path, '/');
    if (p == NULL || *(p + 1) == '\0')
        strcpy(filename, "index.html");
    else
        snprintf(filename, sizeof(filename), "%s", p + 1);

    fp = fopen(filename, "wb");
    if (fp == NULL) {
        perror("File error");
        free(data);
        return 1;
    }

    fwrite(body, 1, size - header, fp);
    fclose(fp);

    printf("HTTP Response: ");
    for (int i = 0; i < size && data[i] != '\r'; i++)
        putchar(data[i]);
    printf("\n");

    printf("Response time: %f seconds\n", response);
    printf("Download time: %f seconds\n", download);
    printf("Data size: %d bytes\n", size - header);
    throughput = download > 0 ? (size - header) / download : 0;
    printf("Throughput: %f bytes/second\n", throughput);
    printf("Saved as %s\n", filename);

    free(data);
    return 0;
}