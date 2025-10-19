//
// Created by emmanuel on 4/10/25.
//
#ifndef SRC_STRUCTS_H
#define SRC_STRUCTS_H
struct Country{
    //Parametros y punteros de los paises
    char* name;
    float corruption;
	int poverty;
	int crime;
	int unemployment;
	int political_stability;
	struct Country* prev;
	struct Country* next;
	struct Country* next_connected;
	struct DoubleLinkedList* connected_countries;
};

struct DoubleLinkedList {
	struct Country *start;
};

struct Country* newCountry (char* name, float corruption);

int eraseDeadCountries (struct DoubleLinkedList* doubleLinkedList);

struct DoubleLinkedList* newDoubleLinkedList ();

int connectDoubleLinkedList (struct DoubleLinkedList* doubleList, struct Country* country );

int fillList(struct DoubleLinkedList* list);

struct DoubleLinkedList* initializeDoubleLinkedList();

struct Country* getCountryByPosition(struct DoubleLinkedList* list, int position);


#endif //SRC_STRUCTS_H0