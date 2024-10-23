/**
 * @file
 * @brief Servidor
 * @author Ana Isabel Quira Fernández <anaquira@unicauca.edu.co>
 * @copyright MIT License
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <signal.h>
#include "protocol.h"

int s;

void handle_signal(int signal){
    printf("Signal %d received, closing connections...\n", signal);
    if (s >= 0){
        close(s);
    }
    exit(EXIT_SUCCESS);
}

int main (int argc, char * argv[]){
    int c;
    struct sockaddr_in addr, client;
    socklen_t client_len = sizeof(client);

    //0. Instalar los manejadores de señales para SIGINT, SIGTERM
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    //1. Obtener un conector -> s = socket(...)
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Error creating socket.");
        exit(EXIT_FAILURE); 
    }

    //Asociar una dirección al socket (ip, puerto) - bind
    memset(&addr, 0, sizeof(struct sockaddr_in));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234); //Puerto
    addr.sin_addr.s_addr = INADDR_ANY; //Cualquier dirección 0.0.0.0

    if(bind(s, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) < 0) {
        perror("Error binding socket.");
        close(s);
        exit(EXIT_FAILURE);
    }

    //poner el sockect disponible (escuchar) - listen
    if(listen(s,10) < 0){
        perror("Error listening.");
        close(s);
        exit(EXIT_FAILURE);
    }

    printf("Server waiting for connections...\n");

    //(bloqueante) esperar por una conexión o cliente c - accept
    c = accept(s, (struct sockaddr *) &client, &client_len);
    if (c < 0){
        perror("Error accepting connection.");
        close(s);
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted.\n");

    //5. (comunicación) 
    while (1)
    {
        //  recibir saludo - read
        if(receive_greeting(c) < 0) {
            printf("Error receiving messagee or client disconnected.\n");
            break;
        }
        //  enviar saludo - write
        if(send_greeting(c) < 0) {
            printf("Error sending reply");
            break;
        }
    }
    
    
    //  ....
    //6. cerrar el socket del cliente c - close
    close(c);
    //7. cerrar el socket del servidor s - close
    close(s);

    exit(EXIT_SUCCESS);
}


