#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h> // strtoul
#include <string.h> // strlen
#include <unistd.h> // close

// Net includes
#include <sys/socket.h> // socket
#include <arpa/inet.h> //socketaddr_in, inet_addr

// poll includes
#include <poll.h>

int socket_fd;

void quit(int code) {
    close(socket_fd);
    exit(code);
}

void error(char *msg) {
    printf("%s : %s (%d)\n", msg, strerror(errno), errno);
    quit(1);
}

void signal_handler() {
    printf("Received signal to quit\n");
    quit(0);
}

int search_for_quit_command(char *cmd) {
    char quit_cmds[3][10] = { "quit", "exit", "q" };
    for (int i = 0 ; i < 3 ; i++) {
        if (!strcmp(cmd, quit_cmds[i])) {
            return 1;
        }
    }
    return 0;
}

int main(int argc , char **argv) {
    // Install signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    char server_ip[20] = "127.0.0.1";
    uint16_t port = 3000;

    if (argc != 1) {
        if (argc != 3) {
            puts("usage : client <server address> <server port>");
            quit(0);
        }
        strcpy(server_ip, argv[1]);
        port = strtoul(argv[2], NULL, 10);
    }

    printf("server = %s:%d\n", server_ip, port);

    struct pollfd socket_fd, stdin_poll;

    stdin_poll.fd = fileno(stdin);
    stdin_poll.events = POLLIN;

    // Create socket and get its file descriptor
    socket_fd.fd = socket(
        AF_INET, // Internet socket
        SOCK_STREAM, // TCP socket
        0 // Use default protocol
    );
    socket_fd.events = POLLIN;

    if (socket_fd.fd == -1) {
        error("Failed to create socket");
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; // Internet address
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(port);

    //Connect to server
    if (connect(socket_fd.fd, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
        char error_msg[50];
        sprintf(error_msg, "Failed to connect to %s:%d", server_ip, port);
        error(error_msg);
    }
    
    puts("Connected");

    char buffer[1024];
    memset(buffer, 0, 1024);

    while (1) {
        int ret = poll(&socket_fd, 1, 10);
        if (ret == 1 && socket_fd.revents & POLLIN) {
            ssize_t read_size = read(socket_fd.fd, buffer, 1024);
            if (read_size == 0) {
                puts("Connection closed by server");
                quit(0);
            }
            else if (read_size > 0) {
                printf("\rReceived %d bytes from server : %s\n", read_size, buffer);
            }
            memset(buffer, 0, 1024);
        }

        ret = poll(&stdin_poll, 1, 10);
        if (ret == 1 && stdin_poll.revents & POLLIN) {
            fgets(buffer, 1024, stdin);
            buffer[strlen(buffer) - 1] = 0;

            if (search_for_quit_command(buffer)) {
                quit(0);
            }

            ssize_t sent_size = send(socket_fd.fd, buffer, strlen(buffer), 0);
            if (sent_size < 0) {
                error("Send failed");
            }
            memset(buffer, 0, 1024);

            printf("Sent %d bytes\n", sent_size);
        }
    }

    quit(0);
}