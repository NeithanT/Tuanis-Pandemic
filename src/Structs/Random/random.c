#include "random.h"
#include <stdio.h>
#include <stdlib.h>

//###############################################################################

/**
 * Calcula los valores de corrupción, de todos los países en la lista de países
 * @param doubleLinkedList
 */
void calculateCorruptionCountryList(struct DoubleLinkedList* doubleLinkedList) {

    if (doubleLinkedList == NULL || doubleLinkedList->start == NULL) {
        printf("ERROR2000: No se ha podido calcular el valor de corrupción de la lista");
        return;
    }
    struct Country* current = doubleLinkedList->start;
    while (current != NULL) {
        calculateCorruption(current);
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


void calculateCorruption(struct Country* country) {

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
int initialCorruption(struct DoubleLinkedList* list) {
    //TODO colocar el srand(time(NULL)); en main
    int modifiedCount = 0; //Parametro para conocer cuantas modificaciones se llevan
    for (int i = 0; i < 9; i++) {
        int modifier = rand() % 21; // Obtener un pais entre los 21 de LATAM
        int higher = rand() % 2;    // Obtener uno de los dos aspectos a modificar
        modifyAspectsCountry(list,modifier,modifiedCount,higher); //Llamar la funcion para modificar
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
int modifyAspectsCountry(struct DoubleLinkedList* list, int position, int modifiedCount, int higher) {
    struct Country *actual = list->start; //Se apunta al inicio
    int current = 0; //La posicion actual
    while (current != position) { //Buscar el pais a modificar
        actual = actual->next;
        current++;
    }
    //Revisar si el actual ya se modifico y si es asi mover a la derecha
    while (actual -> next != NULL && actual -> poverty != 0) {
        actual = actual -> next;
    }

    //Revisar si el actual ya se modifico y si es asi mover a la izquierda por si ya se llenaron a la derecha
    while (actual -> prev != NULL && actual -> poverty != 0) {
        actual = actual -> prev;
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
void modifyAspectsAfterTurn(struct DoubleLinkedList *list, int position, int change) {
    struct Country *actual = list->start; //Apuntamos al actual
    int current = 0;
    while (current != position) { //Buscar el pais actual
        actual = actual->next;
        current++;
    }

    switch (change) {
        case 0:
            if (actual -> poverty == 3) { //Revisar si el pais ya tiene el aspecto al maximo entonces agregar a ambos lados
                printf("Se intento sumar 1 al pais %s pero su aspecto pobreza esta al maximo\n", actual -> name);
                //Buscar el pais a la izquierda sin la pobreza al maximo y agregarle 1
                while (actual -> prev != NULL && actual -> prev -> poverty == 3) {

                    actual = actual -> prev;
                }

            	if (actual -> prev != NULL && actual -> prev -> poverty != 3) {
            		actual -> prev -> poverty += 1;
            		printf("Se agrego 1 al aspecto pobreza y ahora su nivel es %d en el pais %s \n" , actual -> prev -> poverty, actual -> prev -> name);
            	}

                //Buscar el pais a la derecha sin la pobreza al maximo y agregarle 1
                while (actual -> next != NULL && actual -> next -> poverty == 3) {
                    actual = actual -> next;
                }

                if (actual -> next != NULL && actual -> next -> poverty != 3) {
                    actual -> next -> poverty += 1;
                    printf("Se agrego 1 al aspecto pobreza y ahora su nivel es %d en el pais %s \n" , actual -> next -> poverty, actual -> next -> name);
                }
				break;
            }

            // Si el pais no tiene al maximo la pobreza se le suma uno nada mas
            else {
	            actual -> poverty += 1;
            	printf("Se agrego 1 al aspecto pobreza y ahora su nivel es %d en el pais %s \n" , actual -> poverty, actual -> name);
            	break;
            }

        case 1:
            // Revisamos si los narcos estan al maximo y si es asi sumamos a los lados
            if (actual -> crime == 3) {
                printf("Se intento sumar 1 al pais %s pero su aspecto narcos esta al maximo\n", actual -> name);

                // Buscar el pais a la izquierda que no tenga los narcos al maximo y sumarle 1
                while (actual -> prev != NULL && actual -> prev -> crime == 3) {
                    actual = actual -> prev;
                }

                if (actual -> prev != NULL && actual -> prev -> crime != 3) {
                    actual -> prev -> crime += 1;
                    printf("Se agrego 1 al aspecto narcos y ahora tiene un nivel de %d, del pais %s \n" , actual -> prev -> crime, actual -> prev -> name);
                }

                // Buscar el pais a la derecha que no tenga los narcos al maximo y sumarle 1
            	while (actual -> next != NULL && actual -> next -> crime == 3) {
            		actual = actual -> next;
            	}

                if (actual -> next != NULL && actual -> next -> crime != 3) {
                    actual -> next -> crime += 1;
                    printf("Se agrego 1 al aspecto narcos y ahora tiene un nivel de %d, del pais %s \n" , actual -> next -> crime, actual -> next -> name);
                }

            }

            //En caso de que no este al maximo los narcos sumarle 1
            else {
	            actual -> crime += 1;
            	printf("Se agrego 1 al aspecto narcos y ahora tiene un nivel de %d, del pais %s \n" , actual -> crime, actual -> name);
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
void randomCorruptAfterTurn(struct DoubleLinkedList* list) {
    for (int i = 0; i < 3; i++) {
        int positionCountryToModify = rand() % 21;
        int valueOfProblematic = rand() % 2;
        modifyAspectsAfterTurn(list,positionCountryToModify,valueOfProblematic);
    }
}

//###############################################################################

void reduceRandomProblem(struct Country* country) {

    if (country == NULL) {
        printf("ERROR2200: No se ha podido reducir el problema");
        return;
    }




}