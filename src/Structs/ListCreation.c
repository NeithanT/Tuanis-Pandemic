//
// Created by emmanuel on 4/10/25.
//
#include "structs.h"

//FUncion para crear un pais
struct Country* newCountry (char* name, float corruption) {

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
struct DoubleLinkedList* newDoubleLinkedList () {

    struct DoubleLinkedList* newDoubleLinkedList = calloc(1, sizeof(struct DoubleLinkedList));
    if (newDoubleLinkedList == NULL) { //Revisar si se creo la lista
        return -1;
    }
    return newDoubleLinkedList;

}

//Funcion para llenar la lista con los paises
//Entradas: La lista doble y el pais a rellenar
int connectDoubleLinkedList (struct DoubleLinkedList* doubleList, struct Country* country ) {

    //Revisar si la lista doble y el pais se crearon
    if (doubleList == NULL || country == NULL) {
        return -1;
    }

    //Revisar si existen elementos en la lista
    if (doubleList -> start == NULL) {
        doubleList -> start = country;
        return 0;
    }

    // Crear dos punteros para unir los paises
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

int fillList (struct DoubleLinkedList* list) {

    connectDoubleLinkedList(list, newCountry("Mexico",0.0));
    connectDoubleLinkedList(list, newCountry("Belice",0.0));
    connectDoubleLinkedList(list, newCountry("Guatemala",0.0));
    connectDoubleLinkedList(list, newCountry("El Salvador",0.0));
    connectDoubleLinkedList(list, newCountry("Honduras",0.0));
    connectDoubleLinkedList(list, newCountry("Nicaragua",0.0));
    connectDoubleLinkedList(list, newCountry("Costa Rica",0.0));
    connectDoubleLinkedList(list, newCountry("Panama",0.0));
    connectDoubleLinkedList(list, newCountry("Colombia",0.0));
    connectDoubleLinkedList(list, newCountry("Venezuela",0.0));
    connectDoubleLinkedList(list, newCountry("Guyana",0.0));
    connectDoubleLinkedList(list, newCountry("Surinam",0.0));
    connectDoubleLinkedList(list, newCountry("GuayanaFrancesa",0.0));
    connectDoubleLinkedList(list, newCountry("Brasil",0.0));
    connectDoubleLinkedList(list, newCountry("Uruguay",0.0));
    connectDoubleLinkedList(list, newCountry("Argentina",0.0));
    connectDoubleLinkedList(list, newCountry("Paraguay",0.0));
    connectDoubleLinkedList(list, newCountry("Bolivia",0.0));
    connectDoubleLinkedList(list, newCountry("Chile",0.0));
    connectDoubleLinkedList(list, newCountry("Peru",0.0));
    connectDoubleLinkedList(list, newCountry("Ecuador",0.0));
    return 0;
}

//Function that provides the length of a double_linked_list -> Used in Player.c
int lengthDoubleLinkedList (struct DoubleLinkedList* doubleLinkedList) {

    if (doubleLinkedList == NULL || doubleLinkedList->start == NULL) {
        printf("ERROR700: CANNOT CALCULATE SIZE OF DOUBLELINKEDLIST");
        return -1;
    }
    int size = 0;
    struct Country* current_country = doubleLinkedList->start;
    while (current_country != NULL) {
        current_country = current_country->next;
        size++;
    }
    return size;
}