//
// Created by emmanuel on 4/10/25.
//

#ifndef SRC_STRUCTS_H
#define SRC_STRUCTS_H
#include <stdio.h>
#include <stdlib.h>
struct Country{
    //Parametros y punteros de los paises
    char* name;
    float corruption;
	int poorness;
	int gangs;
	struct Country *prev;
	struct Country *next;
};

struct DoubleLinkedList {
	struct Country *start;
};

struct Country* newCountry (char* name, float corruption);

bool eraseDeadCountries (struct DoubleLinkedList* doubleLinkedList);

struct DoubleLinkedList* newDoubleLinkedList ();

int connectDoubleLinkedList (struct DoubleLinkedList* doubleList, struct Country* country );

int fillList (struct DoubleLinkedList* list);

struct DoubleLinkedList* initializeDoubleLinkedList();

int lengthDoubleLinkedList (struct DoubleLinkedList* doubleLinkedList);

struct DoubleLinkedList* initializeDoubleLinkedList();

void printDoubleLinkedList (struct DoubleLinkedList* doubleLinkedList);

int lengthDoubleLinkedList (struct DoubleLinkedList* doubleLinkedList);




#endif //SRC_STRUCTS_H0