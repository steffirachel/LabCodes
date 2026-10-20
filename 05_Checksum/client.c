#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int s, a, b, sum, checksum;
    struct sockaddr_in server = {0};

    s = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(s, (struct sockaddr *)&server, sizeof(server));

    printf("Enter two 8-bit numbers: ");
    scanf("%d%d", &a, &b);

    sum = a + b;
    while (sum > 255)
        sum = (sum & 255) + (sum >> 8);
    checksum = 255 - sum;

    write(s, &a, sizeof(a));
    write(s, &b, sizeof(b));
    write(s, &checksum, sizeof(checksum));

    close(s);
    return 0;
}
