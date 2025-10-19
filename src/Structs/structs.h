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

struct Country* new_country (char* name, float corruption);

struct DoubleLinkedList* new_double_linked_list();

struct DoubleLinkedList* initialize_double_linked_list();

struct Country* get_country_by_position(struct DoubleLinkedList* list, int position);

int erase_dead_countries(struct DoubleLinkedList* doubleLinkedList);

int connect_double_linked_list(struct DoubleLinkedList* doubleList, struct Country* country );

int fill_list(struct DoubleLinkedList* list);

int length_double_linked_list(struct DoubleLinkedList* doubleLinkedList);

#endif //SRC_STRUCTS_H0