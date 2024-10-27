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
#include <string.h>

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
    Message msg;

    while (1)
    {
        printf("Enter a command: ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
        {
            break;
        }

        buffer[strcspn(buffer, "\n")] = 0;

        if (strncmp(buffer, "add", 3) == 0)
        {
            msg.command = CMD_ADD;

            char *filename = strtok(buffer + 4, " ");
            char *comment = strtok(NULL, " ");

            if(filename && comment)
            {
                strncpy(msg.filename, filename, sizeof(msg.filename)-1);
                strncpy(msg.comment, comment, sizeof(msg.comment)-1);

                handle_add(&msg);
            } 
            else
            {
                printf("Invalid command.\n Usage: add <filename> \"<comment>\"\n");
                continue;
            }
            continue;
        } else if (strncmp(buffer, "list", 4) == 0)
        {
            msg.command = CMD_LIST;

            char *filename = strtok(buffer + 5, " ");

            if(filename)
            {
                strncpy(msg.filename, filename, sizeof(msg.filename)-1);
                handle_list(&msg);
            } 
            else
            {
                printf("Invalid command.\n Usage: list <filename>\n");
                continue;
            }
            continue;

        } else if (strncmp(buffer, "get", 3) == 0)
        {
            msg.command = CMD_GET;

            char *versions= strtok(buffer + 4, " ");
            char *filename = strtok(NULL, " ");

            if(filename && versions)
            {
                strncpy(msg.filename, filename, sizeof(msg.filename)-1);
                msg.versions = atoi(versions);

                handle_get(&msg);
            } 
            else
            {
                printf("Invalid command.\n Usage: get <filename> <version>\n");
                continue;
            }
            continue;

        } else if (strncmp(buffer, "list_all", 8) == 0)
        {
            msg.command = CMD_LIST_ALL;
            handle_list_all();
            continue;;
        } else
        {
            printf("Invalid command.\n");
            continue;
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

    }

    // 4. Cerrar el conector
    close(s);

    return 0;
}
