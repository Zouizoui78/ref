#include <errno.h>
#include <stdio.h>
#include <stdlib.h> // strtoul
#include <string.h> // strlen
#include <unistd.h> // close

// Net includes
#include <sys/socket.h> // socket
#include <arpa/inet.h> //socketaddr_in, inet_addr

void error(char *msg) {
    printf("%s : %s\n", msg, strerror(errno));
    exit(1);
}

int main(int argc , char **argv) {
    char server_ip[20] = "127.0.0.1";
    uint16_t port = 3000;

    if (argc != 1) {
        strcpy(server_ip, argv[1]);
        port = strtoul(argv[2], NULL, 10);
    }

    printf("server = %s:%d\n", server_ip, port);
    
    // Create socket and get its file descriptor
    int socket_fd = socket(
        AF_INET, // Internet socket
        SOCK_STREAM, // TCP socket
        0 // Use default protocol
    );

    if (socket_fd == -1) {
        error("Failed to create socket");
    }

    struct sockaddr_in server_addr;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    //Connect to remote server
    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
        char error_msg[50];
        sprintf(error_msg, "Failed to connect to %s:%d", server_ip, port);
        error(error_msg);
    }
    
    puts("Connected");
    
    //Send some data
    char *message = "Hello !";
    if (send(socket_fd, message, strlen(message), 0) < 0) {
        error("Send failed");
    }

    puts("Sent data");

    close(socket_fd);
    
    return 0;
}