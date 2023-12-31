#include <errno.h>
#include <signal.h>
#include <stdint.h> // uint16_t
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strerror
#include <unistd.h> // read, close

// Net includes
#include <sys/socket.h> // socket
#include <arpa/inet.h> // sockaddr_in, inet_ntop

// poll includes
#include <poll.h>

#include "constants.h"

struct pollfd server_fd;

#define NCLIENTS 10
struct pollfd clients_fds[NCLIENTS];

struct client {
    char name[100];
    char ip[INET_ADDRSTRLEN];
    int fd;
};

struct client clients[NCLIENTS];

void reset_client(int index) {
    if (clients_fds[index].fd != -1) {
        close(clients_fds[index].fd);
    }
    clients_fds[index].fd = -1;

    clients[index].fd = -1;
    memset(clients[index].name, 0, 100);
    memset(clients[index].ip, 0, INET_ADDRSTRLEN);
}

void quit(int code) {
    for (int i = 0 ; i < NCLIENTS ; i++) {
        reset_client(i);
    }
    close(server_fd.fd);
    exit(code);
}

void error(char *msg) {
    printf("%s : %s (%d)\n", msg, strerror(errno), errno);
    quit(EXIT_FAILURE);
}

void signal_handler() {
    printf("Received signal to quit\n");
    quit(EXIT_SUCCESS);
}

int get_new_client_index() {
    int index = -1;
    for (int i = 0 ; i < NCLIENTS ; i++) {
        if (clients_fds[i].fd == -1) {
            index = i;
            break;
        }
    }
    return index;
}

void print_clients_fd() {
    for (int i = 0 ; i < NCLIENTS ; i++) {
        printf("client %d fd = %d\n", i, clients_fds[i].fd);
    }
}

int new_client() {
    int index = get_new_client_index();
    if (index == -1) {
        puts("Cannot accept more connections");
        return -1;
    }

    printf("New client index = %d\n", index);

    struct sockaddr_in socket_addr;
    int socket_addr_len = sizeof(socket_addr);

    // Accept connection
    clients_fds[index].fd = accept(
        server_fd.fd,
        (struct sockaddr *)&socket_addr, // Client addr goes here
        (socklen_t *)&socket_addr_len
    );

    if (clients_fds[index].fd == -1) {
        return -1;
    }

    printf("New connection fd = %d\n", clients_fds[index].fd);
    clients[index].fd = clients_fds[index].fd;

    // Convert internet address (inet) from network (n) form to "presentation" (p) form -> inet_ntop
    if (!inet_ntop(AF_INET, &socket_addr.sin_addr.s_addr, clients[index].ip, INET_ADDRSTRLEN)) {
        puts("Failed to convert client address to string, disconnecting it");
        reset_client(index);
        return -1;
    }
    else {
        printf("New connection from %s\n", clients[index].ip);
        return 0;
    }
}

int check_for_new_client() {
    int ret = poll(&server_fd, 1, 10);

    if (ret == -1) {
        puts("Failed to poll server fd");
        return -1;
    }
    
    if (ret == 1 && server_fd.revents & POLLIN) {
        return 1;
    }

    return 0;
}

int broadcast_message(char *msg, struct client *dest_exclude) {
    int count = 0;
    for (int i = 0 ; i < NCLIENTS ; i++) {
        if (clients[i].fd == -1 || (dest_exclude && clients[i].fd == dest_exclude->fd)) {
            continue;
        }

        ssize_t sent_size = write(clients[i].fd, msg, strlen(msg));
        // msg should not be empty so we treat == 0 as an error
        if (sent_size <= 0) {
            printf("Broadcast to %s failed\n", clients[i].name);
        }
        else {
            count++;
        }
    }
    return count;
}

void new_message(int new_messages) {
    printf("New message from %d fd\n", new_messages);

    int count = 0;
    char recv_buffer[MAX_MESSAGE_SIZE] = "";
    char send_buffer[1024] = "";
    for (int i = 0 ; i < NCLIENTS && count < new_messages ; i++) {
        if (!(clients_fds[i].revents & POLLIN)) {
            continue;
        }

        memset(recv_buffer, 0, MAX_MESSAGE_SIZE);
        memset(send_buffer, 0, 1024);
        count++;

        printf("Reading data from fd %d (%s)\n", i, clients[i].name);
        ssize_t read_size = read(clients_fds[i].fd, recv_buffer, MAX_MESSAGE_SIZE);
        if (read_size == -1) {
            printf("Failed to read data from %s", clients[i].name);
            continue;
        }

        // 0 means connection closed
        else if (read_size == 0) {
            printf("Connection closed by %s\n", clients[i].name);
            sprintf(send_buffer, "%s left the chat", clients[i].name);
            reset_client(i);
            broadcast_message(send_buffer, NULL);
            continue;
        }

        printf("Received %ld bytes from fd %d (%s) : %s", read_size, i, clients[i].name, recv_buffer);
        if (recv_buffer[read_size - 1] != '\n') {
            puts("");
        }

        if (clients[i].name[0]) {
            sprintf(send_buffer, "%s :\n%s", clients[i].name, recv_buffer);
            int ret = broadcast_message(send_buffer, clients + i);
            printf("Broadcasted message to %d clients\n", ret);
        }
        else {
            strcpy(clients[i].name, recv_buffer);
            printf("fd %d name = %s\n", i, clients[i].name);
            sprintf(send_buffer, "%s joined the chat", clients[i].name);
            broadcast_message(send_buffer, NULL);
        }
    }
}

int main(int argc, char **argv) {
    // Install signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    uint16_t port = 3000;
    if (argc == 2) {
        port = strtoul(argv[1], NULL, 10);
    }

    printf("Port = %d\n", port);

    for (int i = 0 ; i < NCLIENTS ; i++) {
        reset_client(i);
        clients_fds[i].events = POLLIN;
    }

    // Create socket and get its file descriptor
    server_fd.fd = socket(
        AF_INET, // Internet socket
        SOCK_STREAM, // TCP socket
        0 // Use default protocol
    );

    if (server_fd.fd == -1) {
        error("Failed to create socket");
    }

    if (setsockopt(server_fd.fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int))) {
        error("Failed to set SO_REUSEADDR socket option");
    }

    server_fd.events = POLLIN;

    // Create ip address used by socket
    struct sockaddr_in socket_addr;
    int socket_addr_len = sizeof(socket_addr);
    socket_addr.sin_family = AF_INET; // Internet address
    socket_addr.sin_addr.s_addr = INADDR_ANY; // Get connection from any address
    socket_addr.sin_port = htons(port); // Convert from machine endianness to network endianness

    // Bind socket to address
    if (bind(server_fd.fd, (struct sockaddr *)&socket_addr, socket_addr_len)) {
        error("Failed to bind socket");
    }

    // Listen for incoming connections
    if (listen(server_fd.fd, SOMAXCONN)) {
        error("Failed to set socket to listen");
    }

    puts("Waiting for connection...");

    while (1) {
        if (check_for_new_client() && new_client()) {
            puts("Failed to accept new connection");
        }

        int ret = poll(clients_fds, NCLIENTS, 10);
        if (ret == -1) {
            puts("Failed to poll clients fds");
            return -1;
        }
        else if (ret > 0) {
            new_message(ret);
        }
    }

    quit(EXIT_SUCCESS);
}