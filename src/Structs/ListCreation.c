//
// Created by emmanuel on 4/10/25.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "structs.h"
#include "Player/Player.h"
#include "Random/random.h"
#include "../Interface/interface.h"

//###############################################################################

// Prototypes
void populate_connections(struct DoubleLinkedList* list);
struct Country* find_country_by_name(struct DoubleLinkedList* list, const char* name);


struct DoubleLinkedList* initialize_double_linked_list() {

    struct DoubleLinkedList* doubleLinkedList = new_double_linked_list();
    fill_list(doubleLinkedList);
    populate_connections(doubleLinkedList);
    initial_corruption(doubleLinkedList);

    return doubleLinkedList;
}

//###############################################################################

//FUncion para crear un pais
struct Country* new_country (char* name, float corruption) {

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
struct DoubleLinkedList* new_double_linked_list () {

    struct DoubleLinkedList* newDoubleLinkedList = calloc(1, sizeof(struct DoubleLinkedList));
    if (newDoubleLinkedList == NULL) { //Revisar si se creo la lista
        return NULL;
    }
    newDoubleLinkedList->connected_count = 0;
    return newDoubleLinkedList;

}

//###############################################################################

//Funcion para conectar a lista conectada (single linked)
void connect_to_connected(struct DoubleLinkedList* list, struct Country* country) {
    if (list == NULL || country == NULL || list->connected_count >= 21) return;
    list->connected_list[list->connected_count++] = country;
}

// Function to remove a country from a connected_countries list
void remove_from_connected(struct DoubleLinkedList* list, struct Country* country) {
    if (list == NULL || country == NULL) return;
    
    // Find the country in the connected list
    for (int i = 0; i < list->connected_count; i++) {
        if (list->connected_list[i] == country) {
            // Shift all remaining countries down
            for (int j = i; j < list->connected_count - 1; j++) {
                list->connected_list[j] = list->connected_list[j + 1];
            }
            list->connected_count--;
            list->connected_list[list->connected_count] = NULL;
            break;
        }
    }
}

// Function to remove a country from all other countries' connected lists
void remove_country_from_all_connections(struct DoubleLinkedList* main_list, struct Country* country_to_remove) {
    if (main_list == NULL || country_to_remove == NULL) return;
    
    struct Country* current = main_list->start;
    while (current != NULL) {
        if (current != country_to_remove && current->connected_countries != NULL) {
            remove_from_connected(current->connected_countries, country_to_remove);
        }
        current = current->next;
    }
}
int connect_double_linked_list(struct DoubleLinkedList* doubleList, struct Country* country) {
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
int erase_dead_countries (struct DoubleLinkedList* doubleLinkedList) {

    int allCountriesErased = 0;

    if (doubleLinkedList == NULL || doubleLinkedList -> start == NULL) {
        printf("ERROR1800: No se la logrado borrar el país");
        return -1;
    }

    struct Country* current = doubleLinkedList -> start;

    //Esta condición verifica si el primer país se corrompio, y cambia el primer país al segundo país en la lista
    if (current->crime == 3 && current->poverty == 3) {
        add_debug_message("☠ %s ha caído ante la corrupción (Crimen: 3/3, Pobreza: 3/3)", current->name);
        
        struct Country* nextCountry = current->next;
        if (nextCountry) {
            nextCountry->prev = NULL;
        }
        doubleLinkedList -> start = nextCountry;
        
        // CRITICAL: Remove this country from all other countries' connected lists
        remove_country_from_all_connections(doubleLinkedList, current);
        
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
            add_debug_message("☠ %s ha caído ante la corrupción (Crimen: 3/3, Pobreza: 3/3)", current->name);
            
            struct Country* previousCountry = current->prev;
            if (previousCountry != NULL) {
                previousCountry->next = NULL;
            }
            
            // CRITICAL: Remove this country from all other countries' connected lists
            remove_country_from_all_connections(doubleLinkedList, current);
            
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
            add_debug_message("☠ %s ha caído ante la corrupción (Crimen: 3/3, Pobreza: 3/3)", current->name);
            
            struct Country* nextCountry = current->next;
            struct Country* previousCountry = current->prev;
            if (previousCountry) {
                previousCountry->next = nextCountry;
            }
            if (nextCountry) {
                nextCountry->prev = previousCountry;
            }
            
            // CRITICAL: Remove this country from all other countries' connected lists
            remove_country_from_all_connections(doubleLinkedList, current);
            
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

int fill_list(struct DoubleLinkedList* list) {

    if (!list) { return 0; }
    connect_double_linked_list(list, new_country("Mexico",0.0));
    connect_double_linked_list(list, new_country("Guatemala",0.0));
    connect_double_linked_list(list, new_country("Belice",0.0));
    connect_double_linked_list(list, new_country("ElSalvador",0.0));
    connect_double_linked_list(list, new_country("Honduras",0.0));
    connect_double_linked_list(list, new_country("Nicaragua",0.0));
    connect_double_linked_list(list, new_country("CostaRica",0.0));
    connect_double_linked_list(list, new_country("Panama",0.0));
    connect_double_linked_list(list, new_country("Colombia",0.0));
    connect_double_linked_list(list, new_country("Venezuela",0.0));
    connect_double_linked_list(list, new_country("Guyana",0.0));
    connect_double_linked_list(list, new_country("Surinam",0.0));
    connect_double_linked_list(list, new_country("GuayanaFrancesa",0.0));
    connect_double_linked_list(list, new_country("Brasil",0.0));
    connect_double_linked_list(list, new_country("Uruguay",0.0));
    connect_double_linked_list(list, new_country("Argentina",0.0));
    connect_double_linked_list(list, new_country("Paraguay",0.0));
    connect_double_linked_list(list, new_country("Bolivia",0.0));
    connect_double_linked_list(list, new_country("Chile",0.0));
    connect_double_linked_list(list, new_country("Peru",0.0));
    connect_double_linked_list(list, new_country("Ecuador",0.0));
    return 0;
}


//###############################################################################

//Function that provides the length of a double_linked_list -> Used in Player.c
int length_double_linked_list (struct DoubleLinkedList* doubleLinkedList) {

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
void populate_connections(struct DoubleLinkedList* list) {
    struct Country* current = list->start;
    while (current != NULL) {
        current->connected_countries = new_double_linked_list();
        current = current->next;
    }

    // Now add connections based on the comments in fillList
    // Mexico: Belize, Guatemala
    struct Country* mexico = find_country_by_name(list, "Mexico");
    if (mexico) {
        connect_to_connected(mexico->connected_countries, find_country_by_name(list, "Belice"));
        connect_to_connected(mexico->connected_countries, find_country_by_name(list, "Guatemala"));
    }

    // Guatemala: Mexico, Belize, Honduras, El Salvador
    struct Country* guatemala = find_country_by_name(list, "Guatemala");
    if (guatemala) {
        connect_to_connected(guatemala->connected_countries, find_country_by_name(list, "Mexico"));
        connect_to_connected(guatemala->connected_countries, find_country_by_name(list, "Belice"));
        connect_to_connected(guatemala->connected_countries, find_country_by_name(list, "Honduras"));
        connect_to_connected(guatemala->connected_countries, find_country_by_name(list, "ElSalvador"));
    }

    // Belize: Mexico, Guatemala
    struct Country* belice = find_country_by_name(list, "Belice");
    if (belice) {
        connect_to_connected(belice->connected_countries, find_country_by_name(list, "Mexico"));
        connect_to_connected(belice->connected_countries, find_country_by_name(list, "Guatemala"));
    }

    // El Salvador: Guatemala, Honduras
    struct Country* el_salvador = find_country_by_name(list, "ElSalvador");
    if (el_salvador) {
        connect_to_connected(el_salvador->connected_countries, find_country_by_name(list, "Guatemala"));
        connect_to_connected(el_salvador->connected_countries, find_country_by_name(list, "Honduras"));
    }

    // Honduras: Guatemala, El Salvador, Nicaragua
    struct Country* honduras = find_country_by_name(list, "Honduras");
    if (honduras) {
        connect_to_connected(honduras->connected_countries, find_country_by_name(list, "Guatemala"));
        connect_to_connected(honduras->connected_countries, find_country_by_name(list, "El Salvador"));
        connect_to_connected(honduras->connected_countries, find_country_by_name(list, "Nicaragua"));
    }

    // Nicaragua: Honduras, Costa Rica
    struct Country* nicaragua = find_country_by_name(list, "Nicaragua");
    if (nicaragua) {
        connect_to_connected(nicaragua->connected_countries, find_country_by_name(list, "Honduras"));
        connect_to_connected(nicaragua->connected_countries, find_country_by_name(list, "CostaRica"));
    }

    // Costa Rica: Nicaragua, Panama
    struct Country* costa_rica = find_country_by_name(list, "CostaRica");
    if (costa_rica) {
        connect_to_connected(costa_rica->connected_countries, find_country_by_name(list, "Nicaragua"));
        connect_to_connected(costa_rica->connected_countries, find_country_by_name(list, "Panama"));
    }

    // Panama: Costa Rica, Colombia
    struct Country* panama = find_country_by_name(list, "Panama");
    if (panama) {
        connect_to_connected(panama->connected_countries, find_country_by_name(list, "CostaRica"));
        connect_to_connected(panama->connected_countries, find_country_by_name(list, "Colombia"));
    }

    // Colombia: Panama, Venezuela, Brasil, Ecuador, Peru
    struct Country* colombia = find_country_by_name(list, "Colombia");
    if (colombia) {
        connect_to_connected(colombia->connected_countries, find_country_by_name(list, "Panama"));
        connect_to_connected(colombia->connected_countries, find_country_by_name(list, "Venezuela"));
        connect_to_connected(colombia->connected_countries, find_country_by_name(list, "Brasil"));
        connect_to_connected(colombia->connected_countries, find_country_by_name(list, "Ecuador"));
        connect_to_connected(colombia->connected_countries, find_country_by_name(list, "Peru"));
    }

    // Venezuela: Colombia, Brasil, Guyana
    struct Country* venezuela = find_country_by_name(list, "Venezuela");
    if (venezuela) {
        connect_to_connected(venezuela->connected_countries, find_country_by_name(list, "Colombia"));
        connect_to_connected(venezuela->connected_countries, find_country_by_name(list, "Brasil"));
        connect_to_connected(venezuela->connected_countries, find_country_by_name(list, "Guyana"));
    }

    // Guyana: Venezuela, Brasil, Surinam
    struct Country* guyana = find_country_by_name(list, "Guyana");
    if (guyana) {
        connect_to_connected(guyana->connected_countries, find_country_by_name(list, "Venezuela"));
        connect_to_connected(guyana->connected_countries, find_country_by_name(list, "Brasil"));
        connect_to_connected(guyana->connected_countries, find_country_by_name(list, "Surinam"));
    }

    // Surinam: Guyana, Brasil, Guayana Francesa
    struct Country* surinam = find_country_by_name(list, "Surinam");
    if (surinam) {
        connect_to_connected(surinam->connected_countries, find_country_by_name(list, "Guyana"));
        connect_to_connected(surinam->connected_countries, find_country_by_name(list, "Brasil"));
        connect_to_connected(surinam->connected_countries, find_country_by_name(list, "GuayanaFrancesa"));
    }

    // Guayana Francesa: Surinam, Brasil
    struct Country* guayana_francesa = find_country_by_name(list, "GuayanaFrancesa");
    if (guayana_francesa) {
        connect_to_connected(guayana_francesa->connected_countries, find_country_by_name(list, "Surinam"));
        connect_to_connected(guayana_francesa->connected_countries, find_country_by_name(list, "Brasil"));
    }

    // Brasil: Guayana Francesa, Surinam, Guyana, Venezuela, Colombia, Peru, Bolivia, Paraguay, Argentina, Uruguay
    struct Country* brasil = find_country_by_name(list, "Brasil");
    if (brasil) {
        connect_to_connected(brasil->connected_countries, find_country_by_name(list, "GuayanaFrancesa"));
        connect_to_connected(brasil->connected_countries, find_country_by_name(list, "Surinam"));
        connect_to_connected(brasil->connected_countries, find_country_by_name(list, "Guyana"));
        connect_to_connected(brasil->connected_countries, find_country_by_name(list, "Venezuela"));
        connect_to_connected(brasil->connected_countries, find_country_by_name(list, "Colombia"));
        connect_to_connected(brasil->connected_countries, find_country_by_name(list, "Peru"));
        connect_to_connected(brasil->connected_countries, find_country_by_name(list, "Bolivia"));
        connect_to_connected(brasil->connected_countries, find_country_by_name(list, "Paraguay"));
        connect_to_connected(brasil->connected_countries, find_country_by_name(list, "Argentina"));
        connect_to_connected(brasil->connected_countries, find_country_by_name(list, "Uruguay"));
    }

    // Uruguay: Brasil, Argentina
    struct Country* uruguay = find_country_by_name(list, "Uruguay");
    if (uruguay) {
        connect_to_connected(uruguay->connected_countries, find_country_by_name(list, "Brasil"));
        connect_to_connected(uruguay->connected_countries, find_country_by_name(list, "Argentina"));
    }

    // Argentina: Uruguay, Brasil, Paraguay, Bolivia, Chile
    struct Country* argentina = find_country_by_name(list, "Argentina");
    if (argentina) {
        connect_to_connected(argentina->connected_countries, find_country_by_name(list, "Uruguay"));
        connect_to_connected(argentina->connected_countries, find_country_by_name(list, "Brasil"));
        connect_to_connected(argentina->connected_countries, find_country_by_name(list, "Paraguay"));
        connect_to_connected(argentina->connected_countries, find_country_by_name(list, "Bolivia"));
        connect_to_connected(argentina->connected_countries, find_country_by_name(list, "Chile"));
    }

    // Paraguay: Argentina, Brasil, Bolivia
    struct Country* paraguay = find_country_by_name(list, "Paraguay");
    if (paraguay) {
        connect_to_connected(paraguay->connected_countries, find_country_by_name(list, "Argentina"));
        connect_to_connected(paraguay->connected_countries, find_country_by_name(list, "Brasil"));
        connect_to_connected(paraguay->connected_countries, find_country_by_name(list, "Bolivia"));
    }

    // Bolivia: Paraguay, Brasil, Peru, Chile, Argentina
    struct Country* bolivia = find_country_by_name(list, "Bolivia");
    if (bolivia) {
        connect_to_connected(bolivia->connected_countries, find_country_by_name(list, "Paraguay"));
        connect_to_connected(bolivia->connected_countries, find_country_by_name(list, "Brasil"));
        connect_to_connected(bolivia->connected_countries, find_country_by_name(list, "Peru"));
        connect_to_connected(bolivia->connected_countries, find_country_by_name(list, "Chile"));
        connect_to_connected(bolivia->connected_countries, find_country_by_name(list, "Argentina"));
    }

    // Chile: Bolivia, Peru, Argentina
    struct Country* chile = find_country_by_name(list, "Chile");
    if (chile) {
        connect_to_connected(chile->connected_countries, find_country_by_name(list, "Bolivia"));
        connect_to_connected(chile->connected_countries, find_country_by_name(list, "Peru"));
        connect_to_connected(chile->connected_countries, find_country_by_name(list, "Argentina"));
    }

    // Peru: Chile, Bolivia, Brasil, Colombia, Ecuador
    struct Country* peru = find_country_by_name(list, "Peru");
    if (peru) {
        connect_to_connected(peru->connected_countries, find_country_by_name(list, "Chile"));
        connect_to_connected(peru->connected_countries, find_country_by_name(list, "Bolivia"));
        connect_to_connected(peru->connected_countries, find_country_by_name(list, "Brasil"));
        connect_to_connected(peru->connected_countries, find_country_by_name(list, "Colombia"));
        connect_to_connected(peru->connected_countries, find_country_by_name(list, "Ecuador"));
    }

    // Ecuador: Peru, Colombia
    struct Country* ecuador = find_country_by_name(list, "Ecuador");
    if (ecuador) {
        connect_to_connected(ecuador->connected_countries, find_country_by_name(list, "Peru"));
        connect_to_connected(ecuador->connected_countries, find_country_by_name(list, "Colombia"));
    }
}

// Helper function to find country by name
struct Country* find_country_by_name(struct DoubleLinkedList* list, const char* name) {
    struct Country* current = list->start;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

struct Country* get_country_by_position(struct DoubleLinkedList* list, int position) {
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