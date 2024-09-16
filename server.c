#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // for close()
#include <arpa/inet.h>
#include "parser.tab.h"
#include "ast.h"

#define PORT 8080

extern int yyparse(void);
extern void set_input_string(const char *str);

extern char output_buffer[];

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[30000] = {0};
    char *body;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options separately
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(EXIT_FAILURE);
    }

    // Optionally set SO_REUSEPORT if available
#ifdef SO_REUSEPORT
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt(SO_REUSEPORT) failed");
        // It's safe to continue if SO_REUSEPORT is not critical
    }
#endif

    // Configure address struct
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // listen on all interfaces
    address.sin_port = htons(PORT);

    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d...\n", PORT);

    while(1) {
        // Accept a new connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t*)&addrlen))<0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        // Read data from the client
        int valread = read(new_socket, buffer, 30000);
        buffer[valread] = '\0';
        printf("Received request:\n%s\n", buffer);

        // Find the body of the HTTP request
        body = strstr(buffer, "\r\n\r\n");
        if (body) {
            body += 4; // Skip the "\r\n\r\n"
            printf("Body:\n%s\n", body);

            // Now parse the SQL in the body
            set_input_string(body);
            output_buffer[0] = '\0'; // Clear the output buffer
            yyparse();

            // Send the AST back to the client
            char response[4096];
            snprintf(response, sizeof(response),
                     "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%s",
                     output_buffer);
            write(new_socket, response, strlen(response));
        } else {
            printf("No body found in request\n");
            char *response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nNo SQL provided.\n";
            write(new_socket, response, strlen(response));
        }

        close(new_socket);
    }

    return 0;
}
