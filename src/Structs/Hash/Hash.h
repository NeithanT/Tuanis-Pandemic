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

struct listSolutions* createNewList();
struct nodeList* createNewNode(char* solution_string);
void insertNode(struct listSolutions* listSolutionsCountry,char* SolutionString);

struct hastTable* createNewHashTable();
struct slotHashTable* createNewSlotHashTable();

void insertHashSlots(struct hastTable* table,char* nameKey, struct listSolutions* list_solutions);
struct slotHashTable* pairKeyNameAndValue(char* keyName, struct listSolutions* listSolutions);


struct listSolutions* searchByKey(struct hastTable* table, char* keyName);
void printHashTable(struct hastTable* table);
void populateHashTable(struct hastTable* table);


void copySolutionsToNewList(struct listSolutions* oldListSolutions, struct listSolutions* newListSolutions);
int lengthSolutionList(struct listSolutions* listSolutions);
void printListSolutions(struct listSolutions* listSolutions);


void freeListSolutions(struct listSolutions* listSolutions);
void freeHashTable(struct hastTable* table);


#endif //TUANIS_PANDEMIC_HASH_H