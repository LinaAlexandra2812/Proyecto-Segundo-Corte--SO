//Comentario prueba
/**
 * @file
 * @brief Implementacion del API de gestion de versiones
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>
 * @copyright MIT License
*/

#include "versions.h"


/**
 * @brief Crea una version en memoria del archivo
 * Valida si el archivo especificado existe y crea su hash
 * @param filename Nombre del archivo
 * @param hash Hash del contenido del archivo
 * @param comment Comentario
 * @param result Nueva version en memoria
 *
 * @return Resultado de la operacion
 */
return_code create_version(char * filename, char * comment, file_version * result);

/**
 * @brief Obtiene el hash de un archivo.
 * @param filename Nombre del archivo a obtener el hash
 * @param hash Buffer para almacenar el hash (HASH_SIZE)
 * @return Referencia al buffer, NULL si ocurre error
 */
char *get_file_hash(char * filename, char * hash);

/**
 * @brief Verifica si existe una version para un archivo
 *
 * @param filename Nombre del archivo
 * @param hash Hash del contenido
 *
 * @return 1 si la version existe, 0 en caso contrario.
 */
int version_exists(char * filename, char * hash);

/**
 * @brief Copia un archivo
 *
 * @param source Archivo fuente
 * @param destination Destino
 *
 * @return
 */
int copy(char * source, char * destination);

/**
* @brief Almacena un archivo en el repositorio
*
* @param filename Nombre del archivo
* @param hash Hash del archivo: nombre del archivo en el repositorio
*
* @return
*/
int store_file(char * filename, char * hash);

/**
* @brief Almacena un archivo en el repositorio
*
* @param hash Hash del archivo: nombre del archivo en el repositorio
* @param filename Nombre del archivo
*
* @return
*/
int retrieve_file(char * hash, char * filename);

/**
 * @brief Adiciona una nueva version de un archivo.
 *
 * @param filename Nombre del archivo.
 * @param comment Comentario de la version.
 * @param hash Hash del contenido.
 *
 * @return 1 en caso de exito, 0 en caso de error.
 */
int add_new_version(file_version * v);

//Implementación -----------------------------------------------------------
return_code create_version(char * filename, char * comment, file_version * result){
	struct stat s;

	// Verifica si el archivo existe y es regular
    if(stat(filename, &s) < 0 || !S_ISREG(s.st_mode)){
        printf("El archivo no existe o no es un archivo regular\n");
        return VERSION_ERROR;
    }

    // Obtener el hash del archivo
    if (!get_file_hash(filename, result->hash)) {
        printf("Error al obtener el hash del archivo\n");
        return VERSION_ERROR;
    }


	//Llenar la estructura de la versión
	strncpy(result->filename, filename, PATH_MAX);
	strncpy(result->comment, comment, COMMENT_SIZE);

	return VERSION_CREATED;
}

return_code add(char * filename, char * comment) {

    file_version v;

    // 1. Crea la nueva version en memoria
    // Si la operacion falla, retorna VERSION_ERROR
    // create_version(filename, comment, &v)
    if(create_version(filename, comment, &v) != VERSION_CREATED){
        return VERSION_ERROR;
    }

    // 2. Verifica si ya existe una version con el mismo hash
    // Retorna VERSION_ALREADY_EXISTS si ya existe
    //version_exists(filename, v.hash)
	printf("Verificando si la version ya existe...\n");
    if(version_exists(filename, v.hash)){
		printf("La version ya existe\n");
        return VERSION_ALREADY_EXISTS;
    }

    // 3. Almacena el archivo en el repositorio.
    // El nombre del archivo dentro del repositorio es su hash (sin extension)
    // Retorna VERSION_ERROR si la operacion falla
    //store_file(filename, v.hash)
	printf("Almacenando el archivo en el repositorio...\n");
    if(store_file(filename, v.hash)){
		printf("Error al almacenar el archivo en el repositorio\n");
        return VERSION_ERROR;
    }

    // 4. Agrega un nuevo registro al archivo versions.db
    // Si no puede adicionar el registro, se debe borrar el archivo almacenado en el paso anterior
    // Si la operacion falla, retorna VERSION_ERROR
    //add_new_version(&v)
	printf("Agregando nueva version en versions.db...\n");
    if(add_new_version(&v) != VERSION_ADDED){
		printf("Error al agregar nueva version en versions.db\n");
        return VERSION_ERROR;
    }

    // Si la operacion es exitosa, retorna VERSION_ADDED
    return VERSION_ADDED;
}

int add_new_version(file_version * v) {
	FILE * fp;
	//Abrir el archivo "versions.db" en modo de escritura
	fp = fopen(".versions/versions.db", "a");
	
	if(fp == NULL){
		return VERSION_ERROR;
	}

	//Escribe la nueva versión en el archivo
	if(fwrite(v, sizeof(file_version), 1, fp) != 1){
		fclose(fp);
		return VERSION_ERROR;
	}

	fclose(fp);
	return VERSION_ADDED;
}


void list(char * filename) {
	//Abre el la base de datos de versiones (versions.db)
	//Muestra los registros cuyo nombre coincide con filename.
	//Si filename es NULL, muestra todos los registros.
	FILE * fp;
	file_version r;
	int cont = 1;

	//Abrir el archivo ".versions/versions.db" en modo lectura
	fp = fopen(".versions/versions.db", "r");
	if(fp == NULL){
		perror("Error al abrir el archivo versions.db");
		return;
	}

	//Leer hasta fin de archivo
	while (fread(&r, sizeof(file_version), 1, fp) == 1) {
        // Si filename es NULL, mostrar todos los registros
        // Si filename no es NULL, mostrar solo los registros que coinciden
        if (filename == NULL || strcmp(r.filename, filename) == 0) {
            printf("%d. %s, %s, %s\n", cont, r.hash, r.filename, r.comment);
            cont++;
        }
    }
	fclose(fp);
}

char *get_file_hash(char * filename, char * hash) {
	char *comando;
	FILE * fp;

	struct stat s;

	//Verificar que el archivo existe y que se puede obtener el hash
	if (stat(filename, &s) < 0 || !S_ISREG(s.st_mode)) {
		perror("stat");
		return NULL;
	}

	sha256_hash_file_hex(filename, hash);

	return hash;

}

int copy(char * source, char * destination) {
	// Copia el contenido de source a destination (se debe usar open-read-write-close, o fopen-fread-fwrite-fclose)
	FILE *f, *df;
	char buf[1024];
	size_t bytes_leidos; // Cantidad de bytes leidos

	//Abrir el archivo fuente
	f = fopen(source, "r");
	if(f == NULL){
		perror("Error al abrir el archivo fuente");
		exit(EXIT_FAILURE);
	}

	//Abrir el archivo destino
	df = fopen(destination, "w");
	if (df == NULL){
		perror("Error al abrir el archivo destino");
		fclose(f); //Se cierra el archivo fuente si no se abre el archivo destino
		return 1;
	}

	//Leer determinada cantidad de bytes en buf
	//Copiar a destino, mientras no sea fin de archivo
	//TODO dependiendo de la familia usada, se debe verificar que se hayan leido datos

	while ((bytes_leidos = fread(buf, 1, sizeof(buf),f)) > 0){
		if (fwrite(buf, 1,bytes_leidos, df) != bytes_leidos){
			perror("Error al escribir en el archivo destino");
			fclose(f);
			fclose(df);
			return 1;
		}
	}

	fclose(f);
	fclose(df);

	return 0; //Retorna 0 indicando que la operación fue exitosa
}

int version_exists(char * filename, char * hash) {

	FILE *fp;
	file_version r;

	//Abrir el archivo ".versions/versions.db" en modo lectura
	fp = fopen(".versions/versions.db", "r");

	if(fp == NULL){
		return 0;
	}

	while(fread(&r, sizeof(file_version), 1, fp) == 1){
		//Comparar el nombre del archivo y el hash
		if(strcmp(r.filename, filename) == 0 && strcmp(r.hash, hash) == 0){
			fclose(fp);
			return 1;
		}
	}

	fclose(fp);
	return 0;
}

int get(char * filename, int version) {
	//1. Abre la BD y busca el registro r que coincide con filename y version
	//retrieve_file(r.hash, r.filename)

	FILE *fp;
	file_version r;
	int cont;

	//Abrir el archivo ".versions/versions.db"
	fp = fopen(".versions/versions.db", "r");

	if(fp == NULL){
		return 0;
	}

	cont = 1;

	//Leer hasta fin de archivo
	while (!feof(fp)){
		
		//Realizar una lectura y validar
		if (fread(&r, sizeof(file_version), 1, fp) != 1){
			break;
		}

		//Buscar el archivo y la version solicitada
		if(strcmp(r.filename,filename) == 0){
			if (cont == version){
				//Copiar el archivo
				retrieve_file(r.hash, r.filename);
				return 1;
			} else {
				//Buscar la siguiente version
				cont++;
			}
		}
	}

	fclose(fp);
	
}


int store_file(char * filename, char * hash) {
	char dst_filename[PATH_MAX];
	snprintf(dst_filename, PATH_MAX, "%s/%s", VERSIONS_DIR, hash);
	return copy(filename, dst_filename);
}

int retrieve_file(char * hash, char * filename) {
	char src_filename[PATH_MAX];
	snprintf(src_filename, PATH_MAX, "%s/%s", VERSIONS_DIR, hash);
	return copy(src_filename, filename);
}

