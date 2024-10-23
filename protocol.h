/**
 * @file
 * @brief Protocolo de comunicación
 * @author Ana Isabel Quira Fernández <anaquira@unicauca.edu.co>
 * @copyright MIT License
 */

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