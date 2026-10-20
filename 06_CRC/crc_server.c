#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int s, c, n, i, j;
    char frame[100], gen[] = "10011";

    s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_port = htons(8081);
    server.sin_addr.s_addr = INADDR_ANY;
    bind(s, (struct sockaddr *)&server, sizeof(server));
    listen(s, 1);

    printf("Waiting for client...\n");
    c = accept(s, NULL, NULL);
    n = recv(c, frame, sizeof(frame) - 1, 0);

    if (n > 0) {
        frame[n] = '\0';
        for (i = 0; i <= (int)strlen(frame) - 5; i++) {
            if (frame[i] == '1') {
                for (j = 0; j < 5; j++) {
                    if (frame[i + j] == gen[j])
                        frame[i + j] = '0';
                    else
                        frame[i + j] = '1';
                }
            }
        }

        if (strchr(frame, '1') == NULL)
            printf("No error\n");
        else
            printf("There is error\n");
    }

    close(c);
    close(s);
    return 0;
}
