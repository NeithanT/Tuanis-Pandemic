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
	NodeCountry *prev;
	NodeCountry *next;
};

struct DoubleLinkedList {
	NodeCountry *start;
};

struct Country newCountry (char* name, float corruption) {}

struct DoubleLinkedList newDoubleLinkedList () {}
#endif //SRC_STRUCTS_H0