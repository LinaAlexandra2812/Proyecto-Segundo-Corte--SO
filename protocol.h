/**
 * @file
 * @brief Protocolo de comunicación
 * @author Ana Isabel Quira Fernández <anaquira@unicauca.edu.co>
 * @copyright MIT License
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#define CMD_ADD 1
#define CMD_LIST 2
#define CMD_GET 3
#define CMD_LIST_ALL 4
#include "versions.h" 

typedef struct {
    int command;
    char filename[256];
    char comment [80];
    char hash [256];
    int versions;
} Message;

/*
typedef struct {
    char filename[256];
    int version;
    char comment[256];
    char content[1024];
} Version;
*/

void serialize_message(const Message *msg, char *buffer);
void deserialize_message(const char *buffer, Message *msg);

void handle_add(Message *msg);
void handle_list(Message *msg);
void handle_get(Message *msg);
void handle_list_all();


/**
 * @brief Envía un mensaje de saludo
 * 
 * @param s Socket al que se envia el mensaje
 * @return int 
 */
int send_greeting(int s);

/**
 * @brief Recibe un mensaje de saludo
 * 
 * @param s Socket del que se recibe el mensaje
 * @return int 
 */
int receive_greeting(int s);

#endif //PROTOCOL_H