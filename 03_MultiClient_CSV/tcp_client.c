#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int s, n;
    char roll[50], result[300];
    struct sockaddr_in server = {0};

    s = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(s, (struct sockaddr *)&server, sizeof(server));

    printf("Enter roll number: ");
    fgets(roll, sizeof(roll), stdin);
    send(s, roll, strlen(roll), 0);

    n = recv(s, result, sizeof(result) - 1, 0);
    if (n > 0) {
        result[n] = '\0';
        printf("%s", result);
    }

    close(s);
    return 0;
}
