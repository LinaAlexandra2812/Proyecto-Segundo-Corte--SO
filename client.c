/**
 * @file
 * @brief Cliente
 * @author Ana Isabel Quira Fernández <anaquira@unicauca.edu.co>
 * @copyright MIT License
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "protocol.h"
#include <signal.h>

#define BUFFER_SIZE 1024

int s;

void handle_signal(int signal)
{
    printf("Signal %d received, closing connections...\n", signal);
    if (s >= 0)
    {
        close(s);
    }
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    struct sockaddr_in addr;

    if (argc != 3)
    {
        fprintf(stderr, "Uso: %s <IP> <puerto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    // 1. Obtener un conector -> s = socket(...)
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        perror("Error creating socket.");
        exit(EXIT_FAILURE);
    }

    // 2. Conectarse a un dirección (IP, puerto) connect SIMILAR A ~bind
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &addr.sin_addr);

    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Error connecting to server.");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    // 3. (comunicación) Enviar y recibir mensajes -> send_greeting(s, ...), receive_greeting(s, ...)
    char buffer[BUFFER_SIZE];
    while (1)
    {
        printf("Enter a command: ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
        {
            break;
        }

        if (write(s, buffer, strlen(buffer)) < 0)
        {
            perror("Error sending message.");
            break;
        }

        int bytes_read = read(s, buffer, BUFFER_SIZE);
        if (bytes_read < 0)
        {
            perror("Error receiving message.");
            break;
        }

        buffer[bytes_read] = '\0';
        printf("Server response: %s\n", buffer);

        // 4. Cerrar el conector
        close(s);

        return 0;
    }
