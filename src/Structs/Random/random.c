#include "random.h"
#include <stdio.h>
#include <stdlib.h>
#include "../../Interface/interface.h"

//###############################################################################

/**
 * Calcula los valores de corrupción, de todos los países en la lista de países
 * @param doubleLinkedList
 */
void calculate_corruption_country_list(struct DoubleLinkedList* doubleLinkedList) {

    if (doubleLinkedList == NULL || doubleLinkedList->start == NULL) {
        printf("ERROR2000: No se ha podido calcular el valor de corrupción de la lista");
        return;
    }
    struct Country* current = doubleLinkedList->start;
    while (current != NULL) {
        calculate_corruption(current);
        current = current->next;
    }

}

//###############################################################################
/**
 * Calcula el % de corrupción que tiene un país, basandose en los
 * valores de crime, poverty, unemployment y political_stability
 * Corruption is a percentage from 0.0 to 1.0
 * @param country
 */


void calculate_corruption(struct Country* country) {

    if (country == NULL) {
        printf("ERROR1200: No se pudo calcular la corrupción");
        return;
    }
    // Each attribute contributes to corruption
    // poverty, crime, unemployment: 0-3 (higher is worse)
    // political_stability: 0-100 (lower is worse)
    float total = 0.0;
    
    // Normalize each attribute to 0-1 scale
    total += (float)country->poverty / 3.0 * 0.25;      // 25% weight
    total += (float)country->crime / 3.0 * 0.25;        // 25% weight
    total += (float)country->unemployment / 3.0 * 0.25; // 25% weight
    total += (1.0 - ((float)country->political_stability / 100.0)) * 0.25; // 25% weight
    
    country->corruption = total;
}

//###############################################################################

/**
 * Llama a la funcion de modificar los aspectos de un pais para el inicio del juego
 * Se modifican los valores de poverty y crime por la funcion de rand, para elegir un pais entre los 21 y se elige cual aspecto modificar
 * @param list
 * @return
 */
int initial_corruption(struct DoubleLinkedList* list) {
    //TODO colocar el srand(time(NULL)); en main
    int modifiedCount = 0; //Parametro para conocer cuantas modificaciones se llevan
    for (int i = 0; i < 9; i++) {
        int modifier = rand() % 21; // Obtener un pais entre los 21 de LATAM
        int higher = rand() % 2;    // Obtener uno de los dos aspectos a modificar
        modify_aspects_country(list,modifier,modifiedCount,higher); //Llamar la funcion para modificar
        modifiedCount++; //Sumar uno a la cantidad de modificaciones
    }
    return 0;
}


//###############################################################################
/**
 * Funcion donde se modifican los valores iniciales
 * @param list
 * @param position
 * @param modifiedCount
 * @param higher
 * @return
 */
int modify_aspects_country(struct DoubleLinkedList* list, int position, int modifiedCount, int higher) {
    struct Country *actual = list->start; //Se apunta al inicio
    int current = 0; //La posicion actual
    while (current != position && actual != NULL) { //Buscar el pais a modificar
        actual = actual->next;
        current++;
    }
    
    if (actual == NULL) return -1; // Safety check
    
    //Revisar si el actual ya se modifico y si es asi buscar otro en la lista
    struct Country *temp = actual;
    while (temp -> next != NULL && temp -> poverty != 0) {
        temp = temp -> next;
    }
    if (temp->poverty == 0) {
        actual = temp;
    } else {
        // Si no encontramos a la derecha, buscar a la izquierda
        temp = actual;
        while (temp -> prev != NULL && temp -> poverty != 0) {
            temp = temp -> prev;
        }
        if (temp->poverty == 0) {
            actual = temp;
        }
    }

    // Modificar si fueron los primeros 3 numeros un 3 o un 2 si se eligio un 1 o 0
    if (modifiedCount < 3) {
        actual->poverty = (higher == 0) ? 3 : 2;
        actual->crime = (higher == 1) ? 3 : 2;
        return 0;
    }

    // Modificar si fueron los primeros 6 numeros un 2 o un 1 si se eligio un 1 o 0
    if (modifiedCount >= 3 && modifiedCount < 6) {
        actual->poverty = (higher == 0) ? 2 : 1;
        actual->crime = (higher == 0) ? 1 : 2;
        return 0;
    }

    // Modificar si fueron los ultimos 3 numeros un 1 a cada aspecto
    actual->poverty = 1;
    actual->crime = 1;
    return 0;
}

//###############################################################################

/**
 * Funcion para modificar los aspectos despues de un turno
 * @param list
 * @param position
 * @param change
 */
void modify_aspects_after_turn(struct DoubleLinkedList *list, int position, int change) {
    struct Country *actual = list->start; //Apuntamos al actual
    int current = 0;
    while (current != position) { //Buscar el pais actual
        actual = actual->next;
        current++;
    }

    switch (change) {
        case 0:
            if (actual -> poverty == 3) { //Si el pais ya tiene el aspecto al maximo, propagar a países conectados
                add_debug_message("  ⚠ %s: Pobreza al máximo (3/3)", actual->name);
                
                // Propagar a países conectados que no tengan pobreza al máximo
                if (actual->connected_countries && actual->connected_countries->connected_count > 0) {
                    for (int i = 0; i < actual->connected_countries->connected_count; i++) {
                        struct Country* connected = actual->connected_countries->connected_list[i];
                        if (connected && connected->poverty < 3) {
                            connected->poverty += 1;
                            add_debug_message("  ↳ Propagado a %s: Pobreza %d/3", connected->name, connected->poverty);
                            break; // Solo propagar a uno
                        }
                    }
                }
				break;
            }

            // Si el pais no tiene al maximo la pobreza se le suma uno nada mas
            else {
	            actual -> poverty += 1;
            	add_debug_message("  ✓ %s: Pobreza aumentó a %d/3", actual->name, actual->poverty);
            	break;
            }

        case 1:
            // Revisamos si los narcos estan al maximo y si es asi propagamos a conectados
            if (actual -> crime == 3) {
                add_debug_message("  ⚠ %s: Crimen al máximo (3/3)", actual->name);

                // Propagar a países conectados que no tengan crimen al máximo
                if (actual->connected_countries && actual->connected_countries->connected_count > 0) {
                    for (int i = 0; i < actual->connected_countries->connected_count; i++) {
                        struct Country* connected = actual->connected_countries->connected_list[i];
                        if (connected && connected->crime < 3) {
                            connected->crime += 1;
                            add_debug_message("  ↳ Propagado a %s: Crimen %d/3", connected->name, connected->crime);
                            break; // Solo propagar a uno
                        }
                    }
                }
				break;
            }

            //En caso de que no este al maximo los narcos sumarle 1
            else {
	            actual -> crime += 1;
            	add_debug_message("  ✓ %s: Crimen aumentó a %d/3", actual->name, actual->crime);
            	break;
            }
    }

}

//###############################################################################

/**
 * Funcion para modificar tres paises despues de un turno
 * @param list
 * @return
 */
void random_corrupt_after_turn(struct DoubleLinkedList* list) {
    if (!list || !list->start) {
        printf("ERROR: Cannot corrupt - list is empty\n");
        return;
    }
    
    int list_length = length_double_linked_list(list);
    if (list_length == 0) return;
    
    add_debug_message("=== TURNO DE CORRUPCIÓN ===");
    for (int i = 0; i < 3; i++) {
        // Get a random position within the actual list length
        int positionCountryToModify = rand() % list_length;
        int valueOfProblematic = rand() % 2;
        
        // Get country name for debugging
        struct Country* target = list->start;
        for (int j = 0; j < positionCountryToModify && target != NULL; j++) {
            target = target->next;
        }
        
        if (target) {
            add_debug_message("[Corrupción %d/3] Objetivo: %s (%s)", 
                   i + 1, target->name, valueOfProblematic == 0 ? "Pobreza" : "Crimen");
        }
        
        modify_aspects_after_turn(list, positionCountryToModify, valueOfProblematic);
    }
    add_debug_message("=== FIN TURNO CORRUPCIÓN ===");
}

//###############################################################################

void reduce_random_problem(struct Country* country) {

    if (country == NULL) {
        printf("ERROR2200: No se ha podido reducir el problema");
        return;
    }

    // Choose a random problem to reduce (0=poverty, 1=crime, 2=unemployment, 3=political_stability)
    int problem_to_reduce = rand() % 4;
    int attempts = 0;
    
    // Try to find a problem that can be reduced
    while (attempts < 4) {
        switch (problem_to_reduce) {
            case 0:
                if (country->poverty > 0) {
                    country->poverty--;
                    printf("Aliado redujo pobreza en %s a %d\n", country->name, country->poverty);
                    return;
                }
                break;
            case 1:
                if (country->crime > 0) {
                    country->crime--;
                    printf("Aliado redujo crimen en %s a %d\n", country->name, country->crime);
                    return;
                }
                break;
            case 2:
                if (country->unemployment > 0) {
                    country->unemployment--;
                    printf("Aliado redujo desempleo en %s a %d\n", country->name, country->unemployment);
                    return;
                }
                break;
            case 3:
                if (country->political_stability < 100) {
                    country->political_stability += 25;
                    if (country->political_stability > 100) country->political_stability = 100;
                    printf("Aliado mejoró estabilidad política en %s a %d\n", country->name, country->political_stability);
                    return;
                }
                break;
        }
        
        // Try next problem
        problem_to_reduce = (problem_to_reduce + 1) % 4;
        attempts++;
    }
    
    printf("Aliado no pudo reducir ningún problema en %s (todos en 0)\n", country->name);
}