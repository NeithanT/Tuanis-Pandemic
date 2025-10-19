//
// Created by seb on 17/10/25.
//

#ifndef TUANIS_PANDEMIC_HASH_H
#define TUANIS_PANDEMIC_HASH_H

#include <string.h>
#include "../Player/Player.h"
#include "../structs.h"
#include <stdlib.h>
#include <stdio.h>

#define TABLE_SIZE 1000

/**
 * @brief La estructura que maneja una colección (lista enlazada) de posibles soluciones.
 */
struct listSolutions {
    struct nodeList* start; /**< El punto de inicio de nuestra lista de soluciones. */
};


/**
 * @brief La unidad básica que guarda una solución específica dentro de la lista.
 */
struct nodeList {
    char* solutionString; /**< El texto real de la solución (ej: "Crear un Ministerio"). */
    struct nodeList* nextNode; /**< Apunta a la siguiente solución en la lista. */
};


/**
 * @brief Un "cubeta" o espacio dentro de la Tabla Hash.
 *
 * Si dos claves caen en el mismo índice (colisión), se enlazan usando el campo `nextSlot`.
 */
struct slotHashTable {

    /**
     * @brief La etiqueta única que identifica el contenido del slot.
     *
     * Se forma combinando el nombre del país y el problema que tiene (ej: "Costa_Rica-Corrupción").
     */
    char* nameKey;

    /**
     * @brief La lista completa de soluciones asociada a la clave de este país/problema.
     */
    struct listSolutions* listSolutionsCountry;

    /**
     * @brief Puntero al siguiente slot en caso de colisión de hash (encadenamiento).
     */
    struct slotHashTable* nextSlot;
};

/**
 * @brief La estructura principal que organiza toda la Tabla Hash.
 */
struct hastTable {
    /**
     * @brief Un arreglo de punteros donde se guardan todos los slots (cubetas).
     */
    struct slotHashTable** slots;
};

/**
 * @brief Crea una nueva lista de soluciones que comienza vacía.
 *
 * @return Puntero a la nueva estructura `listSolutions` inicializada.
 */
struct listSolutions* create_new_list();

/**
 * @brief Crea un nuevo contenedor (nodo) con el texto de solución que le pases.
 *
 * @param solution_string Cadena de la solución que se almacenará en el nodo.
 * @return Puntero al nuevo nodo `nodeList` creado.
 */
struct nodeList* create_new_node(char* solution_string);

/**
 * @brief Inserta una solución (nodo) dentro de una lista de soluciones existente.
 *
 * Agrega el nuevo nodo al inicio o al final de la lista enlazada de soluciones.
 *
 * @param listSolutionsCountry Puntero a la lista de soluciones donde se insertará.
 * @param SolutionString Cadena de la solución a insertar.
 */
void insert_node(struct listSolutions* listSolutionsCountry,char* SolutionString);

/**
 * @brief Crea una nueva tabla hash y prepara todos sus espacios.
 *
 * Asigna la memoria necesaria para la tabla y se asegura de que todos
 * los slots estén apuntando a `NULL` al inicio.
 *
 * @return Puntero a la nueva estructura `hastTable`.
 */
struct hastTable* create_new_hash_table();

/**
 * @brief Crea un nuevo espacio (slot) para ser usado en la tabla hash.
 *
 * Asigna la memoria para un `slotHashTable` y lo inicializa.
 *
 * @return Puntero al nuevo slot `slotHashTable` inicializado.
 */
struct slotHashTable* create_new_slot_hash_table();

/**
 * @brief Inserta un slot completo (con su clave y lista de soluciones) en la Tabla Hash.
 *
 * Primero calcula dónde debe ir usando el índice hash y luego lo inserta,
 * manejando cualquier colisión por encadenamiento.
 *
 * @param table Puntero a la tabla hash donde se insertará el slot.
 * @param nameKey Clave del slot (nombre del país + problemática).
 * @param list_solutions Puntero a la lista de soluciones a asociar con la clave.
 */
void insert_hash_slots(struct hastTable* table,char* nameKey, struct listSolutions* list_solutions);

/**
 * @brief Crea un nuevo par de Clave-Valor (Slot) para meter en la tabla.
 *
 * Simplemente empaca la clave (`keyName`) y la lista de soluciones (`listSolutions`)
 * dentro de una nueva estructura `slotHashTable`.
 *
 * @param keyName Clave del slot.
 * @param listSolutions Puntero a la lista de soluciones.
 * @return Puntero al slot `slotHashTable` creado.
 */

struct slotHashTable* pair_key_name_and_value(char* keyName, struct listSolutions* listSolutions);

/**
 * @brief Busca una lista de soluciones usando su clave dentro de la Tabla Hash.
 *
 * Calcula el índice hash y luego recorre los slots enlazados hasta encontrar
 * la clave que coincide.
 *
 * @param table Puntero a la tabla hash.
 * @param keyName Clave a buscar.
 * @return Puntero a la lista de soluciones (`listSolutions`) encontrada.
 * @return **NULL** si la clave no está en la tabla.
 */
struct listSolutions* search_by_key(struct hastTable* table, char* keyName);

/**
 * @brief Muestra por consola todo el contenido de la Tabla Hash.
 *
 * Recorre todos los slots y sus listas enlazadas para imprimir las claves y
 * las soluciones asociadas.
 *
 * @param table Puntero a la tabla hash a imprimir.
 */
void print_hash_table(struct hastTable* table);

/**
 * @brief Rellena la tabla hash con las soluciones iniciales del juego.
 *
 * Agrega las claves básicas (país/problema) y sus listas de soluciones predefinidas.
 *
 * @param table Puntero a la tabla hash a poblar.
 */
void populate_hash_table(struct hastTable* table);

/**
 * @brief Hace una copia profunda de todas las soluciones de una lista a otra.
 *
 * Crea nuevos nodos y copia el texto de las soluciones para que la lista
 * de destino sea independiente de la original.
 *
 * @param oldListSolutions Puntero a la lista original (fuente).
 * @param newListSolutions Puntero a la nueva lista (destino).
 */
void copy_solutions_to_new_list(struct listSolutions* oldListSolutions, struct listSolutions* newListSolutions);

/**
 * @brief Devuelve la cantidad de soluciones que hay en una lista.
 *
 * @param listSolutions Puntero a la lista.
 * @return Número de elementos (nodos) en la lista.
 */
int length_solution_list(struct listSolutions* listSolutions);

/**
 * @brief Muestra en pantalla el texto de cada solución en la lista.
 *
 * Recorre cada nodo e imprime su cadena de solución.
 *
 * @param listSolutions Puntero a la lista a imprimir.
 */
void print_list_solutions(struct listSolutions* listSolutions);

/**
 * @brief Libera toda la memoria que ocupa una lista de soluciones y sus nodos.
 *
 * @param listSolutions Puntero a la lista a liberar.
 */

void free_list_solutions(struct listSolutions* listSolutions);

/**
 * @brief Libera toda la memoria utilizada por la Tabla Hash.
 *
 * Se encarga de liberar todas las listas de soluciones, los slots individuales,
 * y finalmente la estructura principal de la tabla para evitar fugas de memoria.
 *
 * @param table Puntero a la tabla hash a liberar.
 */

void free_hash_table(struct hastTable* table);


#endif //TUANIS_PANDEMIC_HASH_H