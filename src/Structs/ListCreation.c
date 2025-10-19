//
// Created by emmanuel on 4/10/25.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "structs.h"
#include "Player/Player.h"
#include "Random/random.h"

//###############################################################################

// Prototypes
void populateConnections(struct DoubleLinkedList* list);
struct Country* findCountryByName(struct DoubleLinkedList* list, const char* name);


struct DoubleLinkedList* initializeDoubleLinkedList() {

    struct DoubleLinkedList* doubleLinkedList = newDoubleLinkedList();
    fillList(doubleLinkedList);
    populateConnections(doubleLinkedList);
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
    newCountry -> poverty = 0;
    newCountry -> crime = 0;
    newCountry -> unemployment = 0;
    newCountry -> political_stability = 100; // Start with good stability

    return newCountry;
}

//###############################################################################

//Funcion para crear una lista
struct DoubleLinkedList* newDoubleLinkedList () {

    struct DoubleLinkedList* newDoubleLinkedList = calloc(1, sizeof(struct DoubleLinkedList));
    if (newDoubleLinkedList == NULL) { //Revisar si se creo la lista
        return NULL;
    }
    newDoubleLinkedList->connected_count = 0;
    return newDoubleLinkedList;

}

//###############################################################################

//Funcion para conectar a lista conectada (single linked)
void connectToConnected(struct DoubleLinkedList* list, struct Country* country) {
    if (list == NULL || country == NULL || list->connected_count >= 21) return;
    list->connected_list[list->connected_count++] = country;
}

int connectDoubleLinkedList(struct DoubleLinkedList* doubleList, struct Country* country) {
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
int eraseDeadCountries (struct DoubleLinkedList* doubleLinkedList) {

    int allCountriesErased = 0;

    if (doubleLinkedList == NULL || doubleLinkedList -> start == NULL) {
        printf("ERROR1800: No se la logrado borrar el país");
        return -1;
    }

    struct Country* current = doubleLinkedList -> start;

    //Esta condición verifica si el primer país se corrompio, y cambia el primer país al segundo país en la lista
    if (current->crime == 3 && current->poverty == 3) {
        struct Country* nextCountry = current->next;
        if (nextCountry) {
            nextCountry->prev = NULL;
        }
        doubleLinkedList -> start = nextCountry;
        
        // Free connected_countries list before freeing the country
        if (current->connected_countries) {
            free(current->connected_countries);
        }
        free(current);
        current = NULL;
        return allCountriesErased;
    }
    
    while (current != NULL) {
        //Esta verificación es únicamente para el último país en la lista de paises (No revisa el país siguiente)
        if (current->next == NULL && current->crime == 3 && current->poverty == 3) {
            struct Country* previousCountry = current->prev;
            if (previousCountry != NULL) {
                previousCountry->next = NULL;
            }
            
            // Free connected_countries list before freeing the country
            if (current->connected_countries) {
                free(current->connected_countries);
            }
            free(current);
            current = NULL;
            return allCountriesErased;
        }
        //Esta verificación elimina el país muerto, y reacomoda los punteros para que todo quede bien en la lista
        else if (current->crime == 3 && current->poverty == 3) {
            struct Country* nextCountry = current->next;
            struct Country* previousCountry = current->prev;
            if (previousCountry) {
                previousCountry->next = nextCountry;
            }
            if (nextCountry) {
                nextCountry->prev = previousCountry;
            }
            
            // Free connected_countries list before freeing the country
            if (current->connected_countries) {
                free(current->connected_countries);
            }
            free(current);
            current = NULL;
            return allCountriesErased;
        }
        // CRITICAL FIX: Advance to next country to avoid infinite loop
        current = current->next;
    }
    return 1;
}
//###############################################################################

int fillList(struct DoubleLinkedList* list) {

    if (!list) { return NULL; }
    connectDoubleLinkedList(list, newCountry("Mexico",0.0));
    connectDoubleLinkedList(list, newCountry("Guatemala",0.0));
    connectDoubleLinkedList(list, newCountry("Belice",0.0));
    connectDoubleLinkedList(list, newCountry("ElSalvador",0.0));
    connectDoubleLinkedList(list, newCountry("Honduras",0.0));
    connectDoubleLinkedList(list, newCountry("Nicaragua",0.0));
    connectDoubleLinkedList(list, newCountry("CostaRica",0.0));
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
        return 0;
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

// Function to populate connected_countries for each country
void populateConnections(struct DoubleLinkedList* list) {
    struct Country* current = list->start;
    while (current != NULL) {
        current->connected_countries = newDoubleLinkedList();
        current = current->next;
    }

    // Now add connections based on the comments in fillList
    // Mexico: Belize, Guatemala
    struct Country* mexico = findCountryByName(list, "Mexico");
    if (mexico) {
        connectToConnected(mexico->connected_countries, findCountryByName(list, "Belice"));
        connectToConnected(mexico->connected_countries, findCountryByName(list, "Guatemala"));
    }

    // Guatemala: Mexico, Belize, Honduras, El Salvador
    struct Country* guatemala = findCountryByName(list, "Guatemala");
    if (guatemala) {
        connectToConnected(guatemala->connected_countries, findCountryByName(list, "Mexico"));
        connectToConnected(guatemala->connected_countries, findCountryByName(list, "Belice"));
        connectToConnected(guatemala->connected_countries, findCountryByName(list, "Honduras"));
        connectToConnected(guatemala->connected_countries, findCountryByName(list, "ElSalvador"));
    }

    // Belize: Mexico, Guatemala
    struct Country* belice = findCountryByName(list, "Belice");
    if (belice) {
        connectToConnected(belice->connected_countries, findCountryByName(list, "Mexico"));
        connectToConnected(belice->connected_countries, findCountryByName(list, "Guatemala"));
    }

    // El Salvador: Guatemala, Honduras
    struct Country* el_salvador = findCountryByName(list, "ElSalvador");
    if (el_salvador) {
        connectToConnected(el_salvador->connected_countries, findCountryByName(list, "Guatemala"));
        connectToConnected(el_salvador->connected_countries, findCountryByName(list, "Honduras"));
    }

    // Honduras: Guatemala, El Salvador, Nicaragua
    struct Country* honduras = findCountryByName(list, "Honduras");
    if (honduras) {
        connectToConnected(honduras->connected_countries, findCountryByName(list, "Guatemala"));
        connectToConnected(honduras->connected_countries, findCountryByName(list, "El Salvador"));
        connectToConnected(honduras->connected_countries, findCountryByName(list, "Nicaragua"));
    }

    // Nicaragua: Honduras, Costa Rica
    struct Country* nicaragua = findCountryByName(list, "Nicaragua");
    if (nicaragua) {
        connectToConnected(nicaragua->connected_countries, findCountryByName(list, "Honduras"));
        connectToConnected(nicaragua->connected_countries, findCountryByName(list, "CostaRica"));
    }

    // Costa Rica: Nicaragua, Panama
    struct Country* costa_rica = findCountryByName(list, "CostaRica");
    if (costa_rica) {
        connectToConnected(costa_rica->connected_countries, findCountryByName(list, "Nicaragua"));
        connectToConnected(costa_rica->connected_countries, findCountryByName(list, "Panama"));
    }

    // Panama: Costa Rica, Colombia
    struct Country* panama = findCountryByName(list, "Panama");
    if (panama) {
        connectToConnected(panama->connected_countries, findCountryByName(list, "CostaRica"));
        connectToConnected(panama->connected_countries, findCountryByName(list, "Colombia"));
    }

    // Colombia: Panama, Venezuela, Brasil, Ecuador, Peru
    struct Country* colombia = findCountryByName(list, "Colombia");
    if (colombia) {
        connectToConnected(colombia->connected_countries, findCountryByName(list, "Panama"));
        connectToConnected(colombia->connected_countries, findCountryByName(list, "Venezuela"));
        connectToConnected(colombia->connected_countries, findCountryByName(list, "Brasil"));
        connectToConnected(colombia->connected_countries, findCountryByName(list, "Ecuador"));
        connectToConnected(colombia->connected_countries, findCountryByName(list, "Peru"));
    }

    // Venezuela: Colombia, Brasil, Guyana
    struct Country* venezuela = findCountryByName(list, "Venezuela");
    if (venezuela) {
        connectToConnected(venezuela->connected_countries, findCountryByName(list, "Colombia"));
        connectToConnected(venezuela->connected_countries, findCountryByName(list, "Brasil"));
        connectToConnected(venezuela->connected_countries, findCountryByName(list, "Guyana"));
    }

    // Guyana: Venezuela, Brasil, Surinam
    struct Country* guyana = findCountryByName(list, "Guyana");
    if (guyana) {
        connectToConnected(guyana->connected_countries, findCountryByName(list, "Venezuela"));
        connectToConnected(guyana->connected_countries, findCountryByName(list, "Brasil"));
        connectToConnected(guyana->connected_countries, findCountryByName(list, "Surinam"));
    }

    // Surinam: Guyana, Brasil, Guayana Francesa
    struct Country* surinam = findCountryByName(list, "Surinam");
    if (surinam) {
        connectToConnected(surinam->connected_countries, findCountryByName(list, "Guyana"));
        connectToConnected(surinam->connected_countries, findCountryByName(list, "Brasil"));
        connectToConnected(surinam->connected_countries, findCountryByName(list, "GuayanaFrancesa"));
    }

    // Guayana Francesa: Surinam, Brasil
    struct Country* guayana_francesa = findCountryByName(list, "GuayanaFrancesa");
    if (guayana_francesa) {
        connectToConnected(guayana_francesa->connected_countries, findCountryByName(list, "Surinam"));
        connectToConnected(guayana_francesa->connected_countries, findCountryByName(list, "Brasil"));
    }

    // Brasil: Guayana Francesa, Surinam, Guyana, Venezuela, Colombia, Peru, Bolivia, Paraguay, Argentina, Uruguay
    struct Country* brasil = findCountryByName(list, "Brasil");
    if (brasil) {
        connectToConnected(brasil->connected_countries, findCountryByName(list, "GuayanaFrancesa"));
        connectToConnected(brasil->connected_countries, findCountryByName(list, "Surinam"));
        connectToConnected(brasil->connected_countries, findCountryByName(list, "Guyana"));
        connectToConnected(brasil->connected_countries, findCountryByName(list, "Venezuela"));
        connectToConnected(brasil->connected_countries, findCountryByName(list, "Colombia"));
        connectToConnected(brasil->connected_countries, findCountryByName(list, "Peru"));
        connectToConnected(brasil->connected_countries, findCountryByName(list, "Bolivia"));
        connectToConnected(brasil->connected_countries, findCountryByName(list, "Paraguay"));
        connectToConnected(brasil->connected_countries, findCountryByName(list, "Argentina"));
        connectToConnected(brasil->connected_countries, findCountryByName(list, "Uruguay"));
    }

    // Uruguay: Brasil, Argentina
    struct Country* uruguay = findCountryByName(list, "Uruguay");
    if (uruguay) {
        connectToConnected(uruguay->connected_countries, findCountryByName(list, "Brasil"));
        connectToConnected(uruguay->connected_countries, findCountryByName(list, "Argentina"));
    }

    // Argentina: Uruguay, Brasil, Paraguay, Bolivia, Chile
    struct Country* argentina = findCountryByName(list, "Argentina");
    if (argentina) {
        connectToConnected(argentina->connected_countries, findCountryByName(list, "Uruguay"));
        connectToConnected(argentina->connected_countries, findCountryByName(list, "Brasil"));
        connectToConnected(argentina->connected_countries, findCountryByName(list, "Paraguay"));
        connectToConnected(argentina->connected_countries, findCountryByName(list, "Bolivia"));
        connectToConnected(argentina->connected_countries, findCountryByName(list, "Chile"));
    }

    // Paraguay: Argentina, Brasil, Bolivia
    struct Country* paraguay = findCountryByName(list, "Paraguay");
    if (paraguay) {
        connectToConnected(paraguay->connected_countries, findCountryByName(list, "Argentina"));
        connectToConnected(paraguay->connected_countries, findCountryByName(list, "Brasil"));
        connectToConnected(paraguay->connected_countries, findCountryByName(list, "Bolivia"));
    }

    // Bolivia: Paraguay, Brasil, Peru, Chile, Argentina
    struct Country* bolivia = findCountryByName(list, "Bolivia");
    if (bolivia) {
        connectToConnected(bolivia->connected_countries, findCountryByName(list, "Paraguay"));
        connectToConnected(bolivia->connected_countries, findCountryByName(list, "Brasil"));
        connectToConnected(bolivia->connected_countries, findCountryByName(list, "Peru"));
        connectToConnected(bolivia->connected_countries, findCountryByName(list, "Chile"));
        connectToConnected(bolivia->connected_countries, findCountryByName(list, "Argentina"));
    }

    // Chile: Bolivia, Peru, Argentina
    struct Country* chile = findCountryByName(list, "Chile");
    if (chile) {
        connectToConnected(chile->connected_countries, findCountryByName(list, "Bolivia"));
        connectToConnected(chile->connected_countries, findCountryByName(list, "Peru"));
        connectToConnected(chile->connected_countries, findCountryByName(list, "Argentina"));
    }

    // Peru: Chile, Bolivia, Brasil, Colombia, Ecuador
    struct Country* peru = findCountryByName(list, "Peru");
    if (peru) {
        connectToConnected(peru->connected_countries, findCountryByName(list, "Chile"));
        connectToConnected(peru->connected_countries, findCountryByName(list, "Bolivia"));
        connectToConnected(peru->connected_countries, findCountryByName(list, "Brasil"));
        connectToConnected(peru->connected_countries, findCountryByName(list, "Colombia"));
        connectToConnected(peru->connected_countries, findCountryByName(list, "Ecuador"));
    }

    // Ecuador: Peru, Colombia
    struct Country* ecuador = findCountryByName(list, "Ecuador");
    if (ecuador) {
        connectToConnected(ecuador->connected_countries, findCountryByName(list, "Peru"));
        connectToConnected(ecuador->connected_countries, findCountryByName(list, "Colombia"));
    }
}

// Helper function to find country by name
struct Country* findCountryByName(struct DoubleLinkedList* list, const char* name) {
    struct Country* current = list->start;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

struct Country* getCountryByPosition(struct DoubleLinkedList* list, int position) {
    if (list == NULL || list->start == NULL || position < 0) return NULL;
    struct Country* current = list->start;
    int i = 0;
    while (current != NULL && i < position) {
        current = current->next;
        i++;
    }
    return current;
}

//###############################################################################