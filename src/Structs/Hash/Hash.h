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

struct listSolutions {
    struct nodeList* start;
};

struct nodeList {
    char* solutionString;
    struct nodeList* nextNode;
};

struct slotHashTable {

    char* nameKey; // Obtiene el valor dependiendo del nombre del país + la problemática a la cual se le bajo el nivel
    struct listSolutions* listSolutionsCountry; //Según la problemática, carga una lista de soluciones para cada país
    struct slotHashTable* nextSlot; //

};

struct hastTable {
    struct slotHashTable** slots;
};

/**
 * Crea una nueva lista de soluciones vacía
 * Puntero a la nueva lista de soluciones
 */
struct listSolutions* create_new_list();

/**
 * Crea un nuevo nodo de lista con la cadena de solución especificada
 * solution_string Cadena de la solución
 * Puntero al nuevo nodo
 */
struct nodeList* create_new_node(char* solution_string);

/**
 * Inserta un nodo con la solución en la lista de soluciones
 * listSolutionsCountry Puntero a la lista de soluciones
 * SolutionString Cadena de la solución a insertar
 */
void insert_node(struct listSolutions* listSolutionsCountry,char* SolutionString);

/**
 * Crea una nueva tabla hash
 * Puntero a la nueva tabla hash
 */
struct hastTable* create_new_hash_table();

/**
 * Crea un nuevo slot para la tabla hash
 * Puntero al nuevo slot
 */
struct slotHashTable* create_new_slot_hash_table();

/**
 * Inserta un slot en la tabla hash con la clave y lista de soluciones
 * table Puntero a la tabla hash
 * nameKey Clave del slot (nombre del país + problemática)
 * list_solutions Puntero a la lista de soluciones
 */
void insert_hash_slots(struct hastTable* table,char* nameKey, struct listSolutions* list_solutions);

/**
 * Crea un par clave-valor para la tabla hash
 * keyName Clave del slot
 * listSolutions Puntero a la lista de soluciones
 * Puntero al slot creado
 */
struct slotHashTable* pair_key_name_and_value(char* keyName, struct listSolutions* listSolutions);

/**
 * Busca una lista de soluciones por clave en la tabla hash
 * table Puntero a la tabla hash
 * keyName Clave a buscar
 * Puntero a la lista de soluciones encontrada, o NULL si no existe
 */
struct listSolutions* search_by_key(struct hastTable* table, char* keyName);

/**
 * Imprime el contenido de la tabla hash
 * table Puntero a la tabla hash
 */
void print_hash_table(struct hastTable* table);

/**
 * Poblada la tabla hash con datos iniciales
 * table Puntero a la tabla hash a poblar
 */
void populate_hash_table(struct hastTable* table);

/**
 * Copia soluciones de una lista a otra
 * oldListSolutions Puntero a la lista original
 * newListSolutions Puntero a la nueva lista
 */
void copy_solutions_to_new_list(struct listSolutions* oldListSolutions, struct listSolutions* newListSolutions);

/**
 * Calcula la longitud de la lista de soluciones
 * listSolutions Puntero a la lista
 * Número de elementos en la lista
 */
int length_solution_list(struct listSolutions* listSolutions);

/**
 * Imprime el contenido de la lista de soluciones
 * listSolutions Puntero a la lista
 */
void print_list_solutions(struct listSolutions* listSolutions);

/**
 * Libera la memoria de la lista de soluciones
 * listSolutions Puntero a la lista a liberar
 */
void free_list_solutions(struct listSolutions* listSolutions);

/**
 * Libera la memoria de la tabla hash
 * table Puntero a la tabla hash a liberar
 */
void free_hash_table(struct hastTable* table);


#endif //TUANIS_PANDEMIC_HASH_H