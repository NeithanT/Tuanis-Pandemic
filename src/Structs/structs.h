//
// Created by emmanuel on 4/10/25.
//
#ifndef SRC_STRUCTS_H
#define SRC_STRUCTS_H
#include <gtk/gtk.h> // necessary  for gdk
struct CountryImage {
    const char *name;
    int x, y;
    int width, height;
    GdkPixbuf *original_pixbuf;
    GdkPixbuf *modified_pixbuf;
};
struct Country {
    //Parametros y punteros de los paises
    char* name;
    float corruption;
	int poverty;
	int crime;
	int unemployment;
	int political_stability;
	struct Country* prev;
	struct Country* next;
    struct CountryImage* image;
	struct DoubleLinkedList* connected_countries;
};

struct DoubleLinkedList {
	struct Country *start;
	int connected_count;
	struct Country* connected_list[21];
};

struct Country* newCountry (char* name, float corruption);

struct DoubleLinkedList* newDoubleLinkedList();

struct DoubleLinkedList* initializeDoubleLinkedList();

struct Country* getCountryByPosition(struct DoubleLinkedList* list, int position);

int eraseDeadCountries(struct DoubleLinkedList* doubleLinkedList);

int connectDoubleLinkedList(struct DoubleLinkedList* doubleList, struct Country* country );

int fillList(struct DoubleLinkedList* list);

int lengthDoubleLinkedList(struct DoubleLinkedList* doubleLinkedList);

#endif //SRC_STRUCTS_H0