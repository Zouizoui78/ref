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

int server_fd, client_fd;

void quit(int code) {
    close(client_fd);
    shutdown(server_fd, SHUT_RDWR);
    exit(code);
}

void error(char *msg) {
    printf("%s : %s\n", msg, strerror(errno));
    quit(1);
}

void signal_handler() {
    printf("Received signal to quit\n");
    quit(0);
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

    // Create socket and get its file descriptor
    server_fd = socket(
        AF_INET, // Internet socket
        SOCK_STREAM, // TCP socket
        0 // Use default protocol
    );

    if (server_fd == -1) {
        error("Failed to create socket");
    }

    // Create ip address used by socket
    struct sockaddr_in socket_addr;
    int socket_addr_len = sizeof(socket_addr);
    socket_addr.sin_family = AF_INET; // Internet address
    socket_addr.sin_addr.s_addr = INADDR_ANY; // Get connection from any address
    socket_addr.sin_port = htons(port); // Convert from machine endianness to network endianness

    // Bind socket to address
    if (bind(server_fd, (struct sockaddr *)&socket_addr, socket_addr_len)) {
        error("Failed to bind socket");
    }

    // Create socket to listen for incoming connections
    // 3 = maximum number of pending connections
    if (listen(server_fd, 3)) {
        error("Failed to set socket to listen");
    }

    puts("Waiting for connection...");

    client_fd = accept(
        server_fd,
        (struct sockaddr *)&socket_addr, // Client addr goes here
        (socklen_t *)&socket_addr_len
    );

    if (client_fd == -1) {
        error("Failed to accept connection");
    }

    char client_addr_str[50];

    // Convert internet address (inet) from network (n) form to "presentation" (p) form -> inet_ntop
    if (!inet_ntop(AF_INET, &socket_addr.sin_addr.s_addr, client_addr_str, INET_ADDRSTRLEN)) {
        error("Failed to convert client address to string");
    }

    printf("New connection from %s, waiting for data...\n", client_addr_str);

    char buffer[1024] = "";

    while (1) {
        ssize_t read_size = read(client_fd, buffer, 1024);

        if (read_size == -1) {
            char error_str[100];
            sprintf(error_str, "Failed to read data from client at %s", client_addr_str);
            error(error_str);
        }

        // 0 means connection closed
        if (read_size == 0) {
            printf("Connection closed by %s\n", client_addr_str);
            break;
        }

        printf("Received %d bytes from %s : %s", read_size, client_addr_str, buffer);
        if (buffer[read_size - 1] != '\n') {
            puts("");
        }

        memset(buffer, 0, 1024);
    }

    quit(0);
}