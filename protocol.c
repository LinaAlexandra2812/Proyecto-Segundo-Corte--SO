/**
 * @file
 * @brief Protocolo de comunicación
 * @author Ana Isabel Quira Fernández <anaquira@unicauca.edu.co>
 * @copyright MIT License
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "protocol.h"

int send_greeting(int s){
    char message[256];

    //Leer el mensaje desde el teclado
    printf("Message to send: ");
    fgets(message, sizeof(message), stdin);
    
    if(write(s, message, strlen(message)) < 0){
        perror("Error sending greeting.");
        return -1;
    }
    printf("Greeting sent: %s\n", message);
    return 0;
}

int receive_greeting(int s){
    char buffer[1024];
    int n = read(s, buffer, sizeof(buffer) - 1);
    
     if (n == 0) {
        printf("Client has disconnected.\n");
        return -1;
    }

    if(n < 0){
        perror("Error receiving greeting.");
        return -1;
    }

    printf("Greeting received: %s\n", buffer);
    return 0;
}
