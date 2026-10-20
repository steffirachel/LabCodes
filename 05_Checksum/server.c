#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int s, c, a, b, checksum, sum;
    struct sockaddr_in server = {0};

    s = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = INADDR_ANY;
    bind(s, (struct sockaddr *)&server, sizeof(server));
    listen(s, 1);

    printf("Waiting for client...\n");
    c = accept(s, NULL, NULL);

    read(c, &a, sizeof(a));
    read(c, &b, sizeof(b));
    read(c, &checksum, sizeof(checksum));

    sum = a + b + checksum;
    while (sum > 255)
        sum = (sum & 255) + (sum >> 8);

    if (sum == 255)
        printf("No error\n");
    else
        printf("There is error\n");

    close(c);
    close(s);
    return 0;
}
