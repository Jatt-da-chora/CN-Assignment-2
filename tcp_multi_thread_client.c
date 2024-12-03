#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

typedef struct {
    int client_id;
} fd;

void *client_task(void *arg) {
    fd *data = (fd *)arg;  
    int client_id = data->client_id;

    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    // char buffer_[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("[Client: %d]: Socket creation error\n", client_id);
        return NULL;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8005);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("[Client: %d]: Invalid address/Address not supported\n", client_id);
        return NULL;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("[Client: %d]: Connection Failed\n", client_id);
        return NULL;
    }

    send(sock, "CPU Statistics", strlen("CPU Statistics"), 0);
    printf("[Client: %d]: Request for CPU Statistics sent.\n", client_id);

    read(sock, buffer, 1024);
    printf("[Client: %d]: Message received from Server: %s\n", client_id, buffer);

    close(sock);
    free(arg);  
    return NULL;
}

int main(int argc, char *argv[]) {
    int clients = 1;

    if (argc > 1) {
        clients = atoi(argv[1]);
    }

    pthread_t *threads = malloc(clients * sizeof(pthread_t));

    for (int i = 0; i < clients; ++i) {
        fd *data = malloc(sizeof(fd));
        data->client_id = i;  

        pthread_create(&threads[i], NULL, client_task, (void *)data);
        printf("[Client: %d]: Connection Request sent to Server.\n", i);
    }

    printf("\n");

    for (int i = 0; i < clients; ++i) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    
    return 0;
}