#include "Play.h"
#include <stdio.h>
#include <stdlib.h>
/**
 * Funcion para consultar los aspectos de cada pais
 * @param list
 * @return
 */
int consultAspects(struct DoubleLinkedList* list) {
    if (list == NULL) { //Revisar estado de la lista
        return 0;
    }

    struct Country *actual = list->start; // Puntero al inicio para imprimir cada pais con todos sus aspectos y su nivel de corrupcion
    while (actual != NULL) {
        printf("Pais: %s. Pobreza: %d, Narcos: %d, Corrupcion \n", actual -> name, actual -> poorness, actual -> gangs, actual -> corruption);
        actual = actual -> next;
    }
    return 0;
}

