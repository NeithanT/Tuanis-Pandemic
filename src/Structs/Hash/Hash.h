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

struct listSolutions* create_new_list();
struct nodeList* create_new_node(char* solution_string);
void insert_node(struct listSolutions* listSolutionsCountry,char* SolutionString);

struct hastTable* create_new_hash_table();
struct slotHashTable* create_new_slot_hash_table();

void insert_hash_slots(struct hastTable* table,char* nameKey, struct listSolutions* list_solutions);
struct slotHashTable* pair_key_name_and_value(char* keyName, struct listSolutions* listSolutions);


struct listSolutions* search_by_key(struct hastTable* table, char* keyName);
void print_hash_table(struct hastTable* table);
void populate_hash_table(struct hastTable* table);


void copy_solutions_to_new_list(struct listSolutions* oldListSolutions, struct listSolutions* newListSolutions);
int length_solution_list(struct listSolutions* listSolutions);
void print_list_solutions(struct listSolutions* listSolutions);


void free_list_solutions(struct listSolutions* listSolutions);
void free_hash_table(struct hastTable* table);


#endif //TUANIS_PANDEMIC_HASH_H