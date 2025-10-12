#include "Play.h"


//###############################################################################
/**
 * Funcion para consultar los aspectos de cada pais
 * @param list
 * @return
 */

int consultAspects(struct DoubleLinkedList* list) {
    if (list == NULL || list -> start == NULL) { //Revisar estado de la lista
        return 0;
    }

    struct Country *actual = list->start; // Puntero al inicio para imprimir cada pais con todos sus aspectos y su nivel de corrupcion
    while (actual != NULL) {
        printf("Pais: %s. Pobreza: %d, Narcos: %d, Corrupcion \n", actual -> name, actual -> poorness, actual -> gangs, actual -> corrupcion);
        actual = actual -> next;
    }
    return 0;
}


//###############################################################################

/**
 * Función utilizada para crear el flujo principal del juego
 * Va ciclando entre el turno del player-corrupción y aliado-corrupción
 * hasta que la validación verifiyWinner sea != 2 (Indicando así un ganador)
 * Llama a la función throwWinner para mostrar el cartelito de ganador
 */
void StartGame() {

    struct DoubleLinkedList* countryList = initializeDoubleLinkedList();
    bool isGameOver = false;
    bool turnoPlayer = true;
    while (!isGameOver) {
        //Turno player
        if (turnoPlayer == true) {
            turnPlayer(countryList);
            turnCorruption(countryList);
            turnoPlayer = false;
        }
        //Turno aliado
        else {
            turnAlly(countryList);
            turnCorruption(countryList);
            turnoPlayer = true;
        }

        //Revisa si hay países para eliminar y si ya se gano el juego
        while (!eraseDeadCountries(countryList)); //Limpia los paises muertos
        if (verifyWinner(countryList) != 2) {
            isGameOver = true;
        }
    } //while
    int winner = verifyWinner(countryList);
    throwWinner(winner);
}


//###############################################################################

void turnPlayer(struct DoubleLinkedList* doubleLinkedList) {


}

//###############################################################################


void turnAlly(struct DoubleLinkedList* doubleLinkedList){}



//###############################################################################
void turnCorruption (struct DoubleLinkedList* doubleLinkedList) {
    randomCorruptAfterTurn(doubleLinkedList);
    calculateCorruptionCountryList(doubleLinkedList);
}


//###############################################################################
/**
 * Primero verifica si ha ganado la corrupción, viendo si el length de la lista es <= 3
 * Luego verifica si ha ganado el jugador, revisando todos los paises en la lista, y continuando
 * únicamente si ese país tiene alguna problemática en 0, en cuyo caso, gana el jugador
 * De lo contrario, simplemente deja de
 * @param doubleLinkedList la lista de países
 * @return (0 si el ganador es el jugador, 1 si es la corrupción, un 2 si nadie ha ganado)
 */
int verifyWinner(struct DoubleLinkedList* doubleLinkedList) {

    if (doubleLinkedList == NULL || doubleLinkedList->start == NULL) {
        printf("ERROR1500: No se ha podido verificar el ganador");
        return -1;
    }
    int winner = 2;
    //Esta condición verifica si quedan 3 o menos paises vivos (Osea que ha ganado la corrupción)
    if (lengthDoubleLinkedList(doubleLinkedList) <= 3) {
        winner = 1;
        return winner; //Indica que ha ganado la corrupción
    }


    //Esta condición verifica si todos los paises vivos tienen un 0 en alguna propiedad.
    struct Country* actual = doubleLinkedList -> start;
    while (actual != NULL) {
        if (actual->gangs != 0 && actual->corruption != 0) { //Verifica que el país tenga alguna problemática != 0
            return winner; //Valor invalido, indica que nadie ha ganado
        }
        actual = actual -> next;

    }
    winner = 0;   //Si ha llegado hasta aquí, es porque todos los paises tienen al menos una problematica en 0
    return winner;

}

//###############################################################################

void throwWinner(int winner) {

    if (!winner) {
        printf("The player has win");
        return;
    }
    printf("The corruption has win");
}

//###############################################################################