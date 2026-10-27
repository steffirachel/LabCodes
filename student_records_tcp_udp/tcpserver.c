#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

void *handleClient(void *arg)
{
    int c = *(int *)arg;
    free(arg);

    char roll[100];
    char msg[1000];
    char data[2000];
    int n;

    n = recv(c, roll, sizeof(roll) - 1, 0);
    if (n <= 0) {
        close(c);
        return NULL;
    }

    roll[n] = '\0';
    roll[strcspn(roll, "\r\n")] = '\0';

    int fd = open("students.csv", O_RDONLY);
    if (fd < 0) {
        strcpy(msg, "Error opening students.csv");
    } else {
        int size = read(fd, data, sizeof(data) - 1);
        close(fd);

        if (size < 0) {
            strcpy(msg, "Error reading students.csv");
        } else {
            data[size] = '\0';
            strcpy(msg, "Student Record Not Found");

            char *line = strtok(data, "\n");
            if (line != NULL)
                line = strtok(NULL, "\n");

            while (line != NULL) {
                char row[300];
                strncpy(row, line, sizeof(row) - 1);
                row[sizeof(row) - 1] = '\0';

                char *field = strtok(row, ",");
                if (field != NULL && strcmp(field, roll) == 0) {
                    snprintf(msg, sizeof(msg), "%s", line);
                    break;
                }

                line = strtok(NULL, "\n");
            }
        }
    }

    send(c, msg, strlen(msg), 0);
    close(c);
    return NULL;
}

int main()
{
    int s;
    struct sockaddr_in server;

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&server, sizeof(server));
    listen(s, 5);

    printf("TCP Server waiting on port 8080...\n");

    while (1) {
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int c = accept(s, (struct sockaddr *)&client, &len);

        if (c >= 0) {
            int *p = malloc(sizeof(int));
            if (p == NULL) {
                close(c);
                continue;
            }

            *p = c;
            pthread_t t;

            if (pthread_create(&t, NULL, handleClient, p) == 0) {
                pthread_detach(t);
            } else {
                free(p);
                close(c);
            }
        }
    }

    close(s);
    return 0;
}
