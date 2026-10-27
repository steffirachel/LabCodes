#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main()
{
    int s;
    struct sockaddr_in server, client;
    socklen_t len;
    char roll[100];
    char data[2000];
    char msg[300];

    s = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8081);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&server, sizeof(server));

    printf("UDP Server waiting on port 8081...\n");

    while (1) {
        len = sizeof(client);

        int n = recvfrom(s, roll, sizeof(roll) - 1, 0,
                         (struct sockaddr *)&client, &len);

        if (n <= 0)
            continue;

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
                        char *name = strtok(NULL, ",");
                        if (name != NULL)
                            snprintf(msg, sizeof(msg), "%s,%s", field, name);
                        break;
                    }

                    line = strtok(NULL, "\n");
                }
            }
        }

        sendto(s, msg, strlen(msg), 0,
               (struct sockaddr *)&client, len);
    }

    close(s);
    return 0;
}
