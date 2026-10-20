#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int s, n;
    char roll[50], line[200], result[100] = "Student Record Not Found";
    struct sockaddr_in server = {0}, client;
    socklen_t len = sizeof(client);
    FILE *fp;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(8081);
    server.sin_addr.s_addr = INADDR_ANY;
    bind(s, (struct sockaddr *)&server, sizeof(server));

    printf("UDP CSV server waiting...\n");
    n = recvfrom(s, roll, sizeof(roll) - 1, 0,
                 (struct sockaddr *)&client, &len);
    if (n > 0) {
        roll[n] = '\0';
        roll[strcspn(roll, "\n")] = '\0';
        fp = fopen("students.csv", "r");
        if (fp != NULL) {
            fgets(line, sizeof(line), fp);
            while (fgets(line, sizeof(line), fp)) {
                char copy[200], *id, *name;
                strcpy(copy, line);
                id = strtok(copy, ",");
                name = strtok(NULL, ",");
                if (id != NULL && strcmp(id, roll) == 0) {
                    snprintf(result, sizeof(result), "%s, %s", id, name ? name : "");
                    result[strcspn(result, "\n")] = '\0';
                    break;
                }
            }
            fclose(fp);
        }
        sendto(s, result, strlen(result), 0,
               (struct sockaddr *)&client, len);
    }
    close(s);
    return 0;
}
