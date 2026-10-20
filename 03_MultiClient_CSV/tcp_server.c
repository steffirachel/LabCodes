#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

void *handle(void *arg)
{
    int c = *(int *)arg;
    char roll[50], line[200], result[300] = "Student Record Not Found";
    FILE *fp;
    int found = 0;

    free(arg);
    recv(c, roll, sizeof(roll) - 1, 0);
    roll[strcspn(roll, "\n")] = '\0';

    fp = fopen("students.csv", "r");
    if (fp != NULL) {
        fgets(line, sizeof(line), fp); /* skip header */
        while (fgets(line, sizeof(line), fp)) {
            char copy[200], *token;
            strcpy(copy, line);
            token = strtok(copy, ",");
            if (token != NULL && strcmp(token, roll) == 0) {
                snprintf(result, sizeof(result), "%s", line);
                found = 1;
                break;
            }
        }
        fclose(fp);
    }

    send(c, result, strlen(result), 0);
    close(c);
    return NULL;
}

int main()
{
    int s;
    struct sockaddr_in server = {0};

    s = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&server, sizeof(server));
    listen(s, 5);
    printf("TCP CSV server waiting...\n");

    while (1) {
        int c = accept(s, NULL, NULL);
        int *p = malloc(sizeof(int));
        pthread_t t;
        *p = c;
        pthread_create(&t, NULL, handle, p);
        pthread_detach(t);
    }
    close(s);
    return 0;
}
