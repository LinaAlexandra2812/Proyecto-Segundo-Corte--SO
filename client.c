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

int s;

void handle_signal(int signal){
    printf("Signal %d received, closing connections...\n", signal);
    if (s >= 0){
        close(s);
    }
    exit(EXIT_SUCCESS);
}

int main (int argc, char * argv[]){
    struct sockaddr_in addr;
    
    if (argc != 3){
        fprintf(stderr, "Uso: %s <IP> <puerto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    //1. Obtener un conector -> s = socket(...)
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0){
        perror("Error creating socket.");
        exit(EXIT_FAILURE);
    }
    //2. Conectarse a un dirección (IP, puerto) connect SIMILAR A ~bind 
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &addr.sin_addr);

    if(connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        perror("Error connecting to server.");
        exit(EXIT_FAILURE);
    }

    //3. (comunicación) Enviar y recibir mensajes -> send_greeting(s, ...), receive_greeting(s, ...)
    while(1){
        if(send_greeting(s)<0) {
            printf("Error sending message.\n");
            break;
        }

        if(receive_greeting(s)<0) {
            printf("Error receiving response.\n");
        }
    }

    //4. Cerrar el conector
    close(s);

    exit(EXIT_SUCCESS);
}


