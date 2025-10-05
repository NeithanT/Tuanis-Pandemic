//
// Created by emmanuel on 4/10/25.
//
#include "structs.h"

//FUncion para crear un pais
struct Country newCountry (char* name, float corruption) {

    struct Country *newCountry = calloc(1, sizeof( struct Country));

    if (newCountry == NULL) { //Revisar si se creo el pais
        return -1;
    }

    //Asignarle el nombre y la corrupcion
    newCountry -> name = name;
    newCountry -> corruption = corruption;

    return newCountry;
}

//Funcion para crear una lista
struct DoubleLinkedList newDoubleLinkedList () {

    struct DoubleLinkedList* newDoubleLinkedList = calloc(1, sizeof(struct DoubleLinkedList));
    if (newDoubleLinkedList == NULL) { //Revisar si se creo la lista
        return -1;
    }
    return newDoubleLinkedList;

}


//Funcion para llenar la lista con los paises
//Entradas: La lista doble y el pais a rellenar
struct DoubleLinkedList fillDoubleLinkedList (struct DoubleLinkedList* doubleList, struct Country* country ) {

    //Revisar si la lista doble y el pais se crearon
    if (doubleList == NULL || country == NULL) {
        return -1;
    }

    //Revisar si existen elementos en la lista
    if (doubleList -> start == NULL) {
        doubleList -> start = country;
    }

    // Crear dos punteror para unir los paises
    struct Country* actual = doubleList -> start;
    struct Country* previous = NULL;

    //Una iteracion para saber donde esta el final
    while (actual != NULL) {
        previous = actual;
        actual = actual -> next;
    }

    //Unir los paises
    previous -> next = country;
    country -> prev = previous;

    return 0;

}