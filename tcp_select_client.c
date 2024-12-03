#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char message[BUFFER_SIZE];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed\n");
        return -1;
    }

    printf("Connected to server. Enter a message ('exit' to disconnect): ");
    while (fgets(message, BUFFER_SIZE, stdin) != NULL) {
        message[strcspn(message, "\n")] = 0;

        if (strcmp(message, "exit") == 0) {
            printf("Disconnecting from server\n");
            break; 
        }

        send(sock, message, strlen(message), 0);
        printf("Message sent\n");

        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread > 0) {
            buffer[valread] = '\0'; 
            printf("Server replied: %s\n", buffer);
        }

        printf("Enter a message: ");
    }

    close(sock);
    return 0;
}
