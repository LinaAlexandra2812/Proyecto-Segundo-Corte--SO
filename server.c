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
#include <sys/stat.h>
#include <pthread.h>

#include "protocol.h"
#include "versions.h"

int s;

void handle_signal(int signal){
    printf("Signal %d received, closing connections...\n", signal);
    if (s >= 0){
        close(s);
    }
    exit(EXIT_SUCCESS);
}

void init_version_system() {
    struct stat st = {0};

    // Verificar si el directorio ".versions" ya existe
    if (stat(".versions", &st) == -1) {
        // Si no existe, crear el directorio
        if (mkdir(".versions", 0700) != 0) {
            perror("Error creating .versions directory");
            exit(EXIT_FAILURE);
        }
    } else if (!S_ISDIR(st.st_mode)) {
        // Si existe pero no es un directorio, mostrar un error
        fprintf(stderr, "Error: .versions exists and is not a directory\n");
        exit(EXIT_FAILURE);
    } else {
        printf(".versions directory already exists.\n");
    }
}

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    free(arg);

    char buffer[1028];
    int bytes_read;

    while ((bytes_read = read(client_socket, buffer, sizeof(buffer))) > 0) {
        buffer[bytes_read] = '\0';
        Message msg;

        deserialize_message(buffer, &msg);

        switch (msg.command) {
            case CMD_ADD:
                handle_add(&msg);
                break;
            case CMD_LIST:
                handle_list(&msg);
                break;
            case CMD_GET:
                handle_get(&msg);
                break;
            default:
                printf("Unknown command\n");
                break; // Añadir break para el caso default
        }
    }

    close(client_socket);
    return NULL;
}

int main (int argc, char * argv[]){
    struct sockaddr_in addr, client;
    socklen_t client_len = sizeof(client);

    //Verificar que el puerto esté proporcionado como argumento
    if(argc != 2){
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);

    //Inicializar el sistema de versiones
    init_version_system();

    //0. Instalar los manejadores de señales para SIGINT, SIGTERM
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    //Crear el directorio de versiones si no existe
    struct stat st = {0};
    if (stat("versions", &st) == -1) {
        if (mkdir("versions", 0700) != 0) {
            perror("Error creating versions directory");
            exit(EXIT_FAILURE);
        }
    }

    //1. Obtener un conector -> s = socket(...)
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Error creating socket.");
        exit(EXIT_FAILURE); 
    }

    //Asociar una dirección al socket (ip, puerto) - bind
    memset(&addr, 0, sizeof(struct sockaddr_in));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[1])); //Puerto
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

    //5. (comunicación) 
    while (1)
    {
        int *c;
        c = malloc(sizeof(int));
        if(c == NULL){
            perror("malloc");
            continue;
        }


        //(bloqueante) esperar por una conexión o cliente c - accept
        *c = accept(s, (struct sockaddr *) &client, &client_len);

        if (*c < 0){
            perror("Error accepting connection.");
            free(c);
            close(s);
            exit(EXIT_FAILURE);
        }

        pthread_t thread_id;
        if(pthread_create(&thread_id, NULL, handle_client, c) != 0){
            perror("pthread_create");
            close(*c);
            free(c);
            continue;
        }

        pthread_detach(thread_id);

        printf("Connection accepted.\n");
    }
    
    //7. cerrar el socket del servidor s - close
    close(s);

    exit(EXIT_SUCCESS);
}


