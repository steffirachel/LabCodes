#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int s, n;
    char roll[50], result[100];
    struct sockaddr_in server = {0};

    s = socket(AF_INET, SOCK_DGRAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(8081);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter roll number: ");
    fgets(roll, sizeof(roll), stdin);
    sendto(s, roll, strlen(roll), 0,
           (struct sockaddr *)&server, sizeof(server));

    n = recvfrom(s, result, sizeof(result) - 1, 0, NULL, NULL);
    if (n > 0) {
        result[n] = '\0';
        printf("%s\n", result);
    }
    close(s);
    return 0;
}
