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
#include <sys/stat.h>
#include "versions.h"

int s;

void handle_signal(int signal){
    printf("Signal %d received, closing connections...\n", signal);
    if (s >= 0){
        close(s);
    }
    exit(EXIT_SUCCESS);
}

void init_version_system(){
    struct stat st;

#ifdef __linux__
    if (mkdir(VERSIONS_DIR, 0755) < 0){
        perror("Error creating versions directory.");
        exit(EXIT_FAILURE);
    }
#elif _WIN32
    if (mkdir(VERSIONS_DIR) < 0){
        perror("Error creating versions directory.");
        exit(EXIT_FAILURE);
    }
#endif

    //Crea el archivo .versions/versions.db si no existe
    if (stat(VERSIONS_DB_PATH, &st) != 0){
        int fd = creat(VERSIONS_DB_PATH, 0755);
        if (fd < 0){
            perror("Error creating versions database.");
            exit(EXIT_FAILURE);
        }
    }


}

void handle_client(int client_socket)
{
    char buffer[1024];
    int bytes_read;
 
    while((bytes_read = read(client_socket, buffer, sizeof(buffer))) > 0)
    {
        buffer[bytes_read] = '\0';
        Message msg;
        
        deserialize_message(buffer, &msg);

        switch (msg.command)
        {
            case CMD_ADD:
                handle_add(&msg);
                break;
            case CMD_LIST:
                handle_list(&msg);
                break;
            case CMD_GET:
                handle_get(&msg);
                break;
            case CMD_LIST_ALL:
                handle_list_all();
                break;
            default:
                printf("Invalid command.\n");
                break;
        }
    }
    
    close(client_socket);
}

int main (int argc, char * argv[]){
    int c;
    struct sockaddr_in addr, client;
    socklen_t client_len = sizeof(client);

    //Verificar que el puerto esté proporcionado como argumento
    if(argc != 2){
        fprintf(stderr, "Uso: %s <puerto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //Inicializar el sistema de versiones
    init_version_system();

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
        //(bloqueante) esperar por una conexión o cliente c - accept
        c = accept(s, (struct sockaddr *) &client, &client_len);
        if (c < 0){
            perror("Error accepting connection.");
            close(s);
            exit(EXIT_FAILURE);
        }

        printf("Connection accepted.\n");

        handle_client(c);
        //6. cerrar el socket del cliente c - close
        close(c);
    }
    
    //7. cerrar el socket del servidor s - close
    close(s);

    exit(EXIT_SUCCESS);
}


