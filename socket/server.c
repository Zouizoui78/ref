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

struct pollfd server_fd;

#define NCLIENTS 10
struct pollfd clients_fds[NCLIENTS];

struct client {
    char name[10];
    char ip[INET_ADDRSTRLEN];
    int fd;
};

struct client clients[NCLIENTS];

void quit(int code) {
    for (int i = 1 ; i < NCLIENTS ; i++) {
        close(clients_fds->fd);
    }
    shutdown(server_fd.fd, SHUT_RDWR);
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
        if (clients_fds[i].fd != 0) {
            continue;
        }
        index = i;
        break;
    }

    if (index == -1) {
        return -1;
    }
}

int print_clients_fd() {
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
        goto error;
    }

    printf("New connection fd = %d\n", clients_fds[index].fd);
    clients[index].fd = clients_fds[index].fd;

    // Convert internet address (inet) from network (n) form to "presentation" (p) form -> inet_ntop
    if (!inet_ntop(AF_INET, &socket_addr.sin_addr.s_addr, clients[index].ip, INET_ADDRSTRLEN)) {
        puts("Failed to convert client address to string");
        goto error;
    }
    else {
        printf("New connection from %s\n", clients[index].ip);
        print_clients_fd();
        return 0;
    }

    error:
    close(clients_fds[index].fd);
    clients_fds[index].fd = 0;
    clients[index].fd = 0;
    return -1;
}

void remove_client(int index) {
    printf("Connection closed by %s\n", clients[index].ip);
    close(clients_fds[index].fd);
    clients_fds[index].fd = 0;
    clients[index].fd = 0;
    print_clients_fd();
}

int check_for_new_client() {
    int ret = poll(&server_fd, 1, 10);

    if (ret == -1) {
        puts("Failed to poll server fd");
        return -1;
    }
    else if (ret == 1 && server_fd.revents & POLLIN) {
        return 1;
    }

    return 0;
}

int broadcast_message(struct client sender, char *msg) {
    int count = 0;
    for (int i = 0 ; i < NCLIENTS ; i++) {
        if (clients[i].fd && clients[i].fd != sender.fd) {
            printf("Broadcasting new message to fd %d\n", i);
            write(clients[i].fd, msg, strlen(msg));
            count++;
        }
    }
    return count;
}

int new_message(int new_messages) {
    printf("New message from %d fd\n", new_messages);

    int count = 0;
    for (int i = 0 ; i < NCLIENTS && count < new_messages ; i++) {
        puts("in loop");
        if (!(clients_fds[i].revents & POLLIN)) {
            continue;
        }

        printf("Reading data from fd %d\n", i);

        char buffer[1024] = "";
        ssize_t read_size = read(clients_fds[i].fd, buffer, 1024);

        if (read_size == -1) {
            char error_str[100];
            sprintf(error_str, "Failed to read data from client at %s", clients[i].ip);
            error(error_str);
        }

        // 0 means connection closed
        else if (read_size == 0) {
            printf("Connection with client %s closed\n", clients[i].ip);
            remove_client(i);
        }
        else {
            printf("Received %d bytes from index %d, ip %s : %s", read_size, i, clients[i].ip, buffer);
            if (buffer[read_size - 1] != '\n') {
                puts("");
            }
            int ret = broadcast_message(clients[i], buffer);
            printf("Broadcasted message to %d clients\n", ret);
        }

        count++;
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
        clients_fds[i].fd = 0;
        clients_fds[i].events = POLLIN;
    }

    // Create socket and get its file descriptor
    server_fd.fd = socket(
        AF_INET, // Internet socket
        SOCK_STREAM, // TCP socket
        0 // Use default protocol
    );

    server_fd.events = POLLIN;
    if (server_fd.fd == -1) {
        error("Failed to create socket");
    }

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

    char buffer[1024] = "";

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