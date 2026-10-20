#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int s, i, j, error;
    char ch, data[9], temp[20], frame[20], gen[] = "10011";
    struct sockaddr_in server = {0};

    printf("Enter one character: ");
    scanf(" %c", &ch);

    for (i = 7; i >= 0; i--)
        data[7 - i] = ((ch >> i) & 1) + '0';
    data[8] = '\0';

    strcpy(temp, data);
    strcat(temp, "0000");

    for (i = 0; i <= (int)strlen(temp) - 5; i++) {
        if (temp[i] == '1') {
            for (j = 0; j < 5; j++)
                temp[i + j] = (temp[i + j] == gen[j]) ? '0' : '1';
        }
    }

    strcpy(frame, data);
    strncat(frame, temp + strlen(temp) - 4, 4);

    printf("Frame: %s\n", frame);
    printf("Introduce error? (1=yes, 0=no): ");
    scanf("%d", &error);
    if (error == 1)
        frame[0] = (frame[0] == '0') ? '1' : '0';

    s = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(8081);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(s, (struct sockaddr *)&server, sizeof(server));
    send(s, frame, strlen(frame), 0);
    close(s);
    return 0;
}
