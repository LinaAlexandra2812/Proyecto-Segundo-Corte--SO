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
#include "versions.h"


void serialize_message(const Message *msg, char *buffer){
    memcpy(buffer, msg, sizeof(Message));
}

void deserialize_message(const char *buffer, Message *msg){
    memcpy(msg, buffer, sizeof(Message));
}

void handle_add(const Message *msg){
    printf("ADD: %s\n", msg->filename);
    add(msg->filename, msg->comment);
}

void  handle_list(const Message *msg){
    printf("LIST: %s\n", msg->filename);
    list(msg->filename);
}

void handle_get(const Message *msg){
    printf("GET: %s\n", msg->filename);
    get(msg->filename, msg->versions);
}

void handle_list_all(){
    printf("LIST ALL\n");
    list(NULL);
}

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
