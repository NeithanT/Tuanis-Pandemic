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

/**
 * Crea un nuevo país con el nombre y nivel de corrupción especificados
 * name Nombre del país
 * corruption Nivel inicial de corrupción (0.0 a 1.0)
 * Puntero al nuevo país creado
 */
struct Country* new_country (char* name, float corruption);

/**
 * Crea una nueva lista doblemente enlazada vacía
 * Puntero a la nueva lista doblemente enlazada
 */
struct DoubleLinkedList* new_double_linked_list();

/**
 * Inicializa y llena la lista doblemente enlazada con países predeterminados
 * Puntero a la lista inicializada
 */
struct DoubleLinkedList* initialize_double_linked_list();

/**
 * Obtiene el país en la posición especificada de la lista
 * list Puntero a la lista doblemente enlazada
 * position Posición del país en la lista (basado en 0)
 * Puntero al país en la posición dada, o NULL si no existe
 */
struct Country* get_country_by_position(struct DoubleLinkedList* list, int position);

/**
 * Elimina países "muertos" de la lista (probablemente aquellos con corrupción alta)
 * doubleLinkedList Puntero a la lista doblemente enlazada
 * Número de países eliminados
 */
int erase_dead_countries(struct DoubleLinkedList* doubleLinkedList);

/**
 * Conecta un país a la lista doblemente enlazada
 * doubleList Puntero a la lista doblemente enlazada
 * country Puntero al país a conectar
 * Código de resultado (probablemente 0 si éxito)
 */
int connect_double_linked_list(struct DoubleLinkedList* doubleList, struct Country* country );

/**
 * Llena la lista con países predeterminados o datos iniciales
 * list Puntero a la lista a llenar
 * Código de resultado
 */
int fill_list(struct DoubleLinkedList* list);

/**
 * Calcula la longitud de la lista doblemente enlazada
 * doubleLinkedList Puntero a la lista
 * Número de elementos en la lista
 */
int length_double_linked_list(struct DoubleLinkedList* doubleLinkedList);

#endif //SRC_STRUCTS_H0