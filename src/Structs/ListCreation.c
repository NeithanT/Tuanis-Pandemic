//
// Created by emmanuel on 4/10/25.
//
#include <signal.h>
#include <stdbool.h>

#include "structs.h"
#include "Player/Player.h"
#include "Random/random.h"

//###############################################################################


struct DoubleLinkedList* initializeDoubleLinkedList() {

    struct DoubleLinkedList* doubleLinkedList = newDoubleLinkedList();
    fillDoubleLinkedList(doubleLinkedList);
    initialCorruption(doubleLinkedList);

    return doubleLinkedList;
}

//###############################################################################

//FUncion para crear un pais
struct Country* newCountry (char* name, float corruption) {

    struct Country *newCountry = calloc(1, sizeof( struct Country));

    if (newCountry == NULL) { //Revisar si se creo el pais
        printf("ERROR1700: No se ha logrado crear el país");
        return NULL;
    }

    //Asignarle el nombre y la corrupcion
    newCountry -> name = name;
    newCountry -> corruption = corruption;

    return newCountry;
}

//###############################################################################

//Funcion para crear una lista
struct DoubleLinkedList* newDoubleLinkedList () {

    struct DoubleLinkedList* newDoubleLinkedList = calloc(1, sizeof(struct DoubleLinkedList));
    if (newDoubleLinkedList == NULL) { //Revisar si se creo la lista
        return -1;
    }
    return newDoubleLinkedList;

}

//###############################################################################

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
//###############################################################################

/**
 * Funciona utilizada para iterar sobre la lista de países, eliminando países hasta que
 * retorne True, indicando que ya no queda ningún país muerto que eliminar, de otra manera, elimina el país
 * y retorna False
 * @param doubleLinkedList
 * @return True si ya se han eliminado todos los paises muertos de la lista, False de lo contrario
 */
bool eraseDeadCountries (struct DoubleLinkedList* doubleLinkedList) {

    bool allCountriesErased = false;

    if (doubleLinkedList == NULL || doubleLinkedList -> start == NULL) {
        printf("ERROR1800: No se la logrado borrar el país");
        return false;
    }

    struct Country* current = doubleLinkedList -> start;


    //Esta condición verifica si el primer país se corrompio, y cambia el primer país al segundo país en la lista
    if (current->gangs == 3 && current->poorness == 3) {
        struct Country* nextCountry = current->next;
        doubleLinkedList -> start = nextCountry;
        nextCountry->prev = doubleLinkedList;
        free(current);
        current = NULL;
        return allCountriesErased;
    }
    while (current != NULL) {
        //Esta verificación es únicamente para el último país en la lista de paises (No revisa el país siguiente)
        if (current->next == NULL && current->gangs == 3 && current->poorness == 3) {
            free(current);
            current = NULL;
            return allCountriesErased;
        }
        //Esta verificación elimina el país muerto, y reacomoda los punteros para que todo quede bien en la lista
        else if (current->gangs == 3 && current->poorness == 3) {
            struct Country* nextCountry = current->next;
            struct Country* previousCountry = current->prev;
            previousCountry->next = nextCountry;
            nextCountry->prev = previousCountry;
            free(current);
            current = NULL;
            return allCountriesErased;
        }
    }
    allCountriesErased = true; //Si llega hasta aqui, es porque no queda ningún país muerto por eliminar
    return allCountriesErased;
}
//###############################################################################

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


//###############################################################################

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


//###############################################################################


/**
 * Funcion para imprimir los paises y conocer sus fronteras
 * @param doubleLinkedList
 */
void printDoubleLinkedList (struct DoubleLinkedList* doubleLinkedList) {
    if (doubleLinkedList == NULL || doubleLinkedList->start == NULL) { // Revisar que la lista no este vacia o no exista
        printf("ERROR700: CANNOT PRINT DOUBLELINKEDLIST");
    }
    struct Country* current_country = doubleLinkedList->start; // Apuntar al inicio
    printf("%s, ==>", current_country -> name ); // Imprimir el primer pais con su unica frontera
    current_country = current_country -> next;
    while (current_country -> next != NULL) { //Imprimir todos los paises con ambas fronteras
        printf("<== %s ==>", current_country -> name);
        current_country = current_country -> next;
    }
    printf("<== %s \n", current_country -> name ); //Imprimir el ultimo pais con su unica frontera
}


//###############################################################################