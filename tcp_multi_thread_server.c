#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include "process_statistics.c"

#define MAX_CLIENTS 1000  

int active_threads = 0; 
int max_clients = 10; 
int client_count = 0; 

typedef struct {
    int new_socket;
    struct sockaddr_in client_addr;
    int client_id;
} fd;

int server_fd; 
struct sockaddr_in server_address; 

fd *client_ids[MAX_CLIENTS];

void handle_sigint(int sig) {
    char server_ip[INET_ADDRSTRLEN], client_ip[INET_ADDRSTRLEN];
    int server_port, client_port;
    
    inet_ntop(AF_INET, &(server_address.sin_addr), server_ip, INET_ADDRSTRLEN);
    server_port = ntohs(server_address.sin_port);  
    
    printf("\nServer shutdown.\n");
    
    for (int i = 0; i < client_count; i++) {
        struct sockaddr_in *client_addr = &client_ids[i]->client_addr;
        inet_ntop(AF_INET, &(client_addr->sin_addr), client_ip, INET_ADDRSTRLEN);
        client_port = ntohs(client_addr->sin_port);  
        
        printf("Client %d: (%s, %d), Server: (%s, %d)\n", i, client_ip, client_port, server_ip, server_port);
        free(client_ids[i]);
    }

    for(int i=0; i<client_count; i++) {
        close(client_ids[i]->new_socket);
    }

    close(server_fd);
    exit(0);
}

void handle_client() {
    while(1){
        char buffer[1024] = {0};
        char hello[1024] = {0};
        
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);

        int new_socket;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            printf("Accept failed");
            exit(EXIT_FAILURE);
        }

        int clientNo = client_count++;
        client_ids[clientNo] = malloc(sizeof(fd));
        client_ids[clientNo]->new_socket = new_socket;
        client_ids[clientNo]->client_addr = client_addr;
        client_ids[clientNo]->client_id = clientNo;


        printf("Client %d connected!\n", clientNo);
        // printf("bsdk\n");
        char *process_stats = getTop();
        // printf("nigger\n");


        read(new_socket, buffer, 1024);
        printf("Request received from Client %d: %s\n", clientNo, buffer);

        strcat(hello, "Request Approved :)\n");
        strcat(hello, process_stats);
        send(new_socket, hello, strlen(hello), 0);
        printf("Request Approved for Client %d.\n", clientNo);
        free(process_stats);
        // printf("gandwe\n");
        close(new_socket);    
        printf("Client %d disconnected.\n\n", clientNo);
    }

}

int main() {
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        printf("Socket failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(8005);

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        printf("setsockopt failed");
        exit(EXIT_FAILURE);
    }


    if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        printf("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, max_clients) < 0) {  
        printf("Listen failed");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_sigint);

    pthread_t threads[max_clients];
    for(int i= 0; i < max_clients; i++) {
        pthread_t thread;
        pthread_create(&thread, NULL, (void *) &handle_client, NULL);
        threads[i] = thread;
    }

    for (int i = 0; i < max_clients; i++) {
        pthread_join(threads[i], NULL);
    }

    close(server_fd);
    return 0;
}
