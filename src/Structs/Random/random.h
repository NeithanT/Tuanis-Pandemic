#ifndef SRC_RANDOM_H
#define SRC_RANDOM_H
#include "../structs.h"

/**
 * Calcula y actualiza el nivel de corrupción de un país
 * country Puntero al país
 */
void calculate_corruption(struct Country* country);

/**
 * Modifica aspectos de un país en la lista (pobreza, crimen, etc.)
 * list Puntero a la lista de países
 * position Posición del país en la lista
 * modifiedCount Número de aspectos a modificar
 * higher Si es 1, aumenta; si 0, disminuye
 * Código de resultado
 */
int modify_aspects_country(struct DoubleLinkedList* list, int position, int modifiedCount, int higher);

/**
 * Establece la corrupción inicial para todos los países en la lista
 * list Puntero a la lista de países
 * Código de resultado
 */
int initial_corruption(struct DoubleLinkedList* list);

/**
 * Calcula la corrupción para todos los países en la lista
 * doubleLinkedList Puntero a la lista de países
 */
void calculate_corruption_country_list(struct DoubleLinkedList* doubleLinkedList);

/**
 * Modifica aspectos de un país después de un turno
 * list Puntero a la lista de países
 * position Posición del país
 * change Cantidad de cambio
 */
void modify_aspects_after_turn(struct DoubleLinkedList* list, int position, int change);

/**
 * Aplica corrupción aleatoria después de un turno a la lista
 * list Puntero a la lista de países
 */
void random_corrupt_after_turn(struct DoubleLinkedList* list);

/**
 * Reduce un problema aleatorio en un país
 * country Puntero al país
 */
void reduce_random_problem(struct Country* country);

#endif //SRC_RANDOM_H