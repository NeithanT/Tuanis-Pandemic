#include "Play.h"


#include "Play.h"


//###############################################################################

int consult_aspects(struct DoubleLinkedList* list) {
    if (list == NULL) { //Revisar estado de la lista
        return 0;
    }

    struct Country *actual = list->start; // Puntero al inicio para imprimir cada pais con todos sus aspectos y su nivel de corrupcion
    while (actual != NULL) {
        printf("Pais: %s. Pobreza: %d, Narcos: %d, Corrupcion \n", actual -> name, actual -> poverty, actual -> crime, actual -> corruption);
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
void start_game() {

    struct DoubleLinkedList* countryList = initialize_double_linked_list();
    struct Player* Player = allocate_initial_player_on_map(countryList);

    bool isGameOver = false;
    while (!isGameOver) {
        turn_player(countryList,Player);
        turn_corruption(countryList);

        //Revisa si hay países para eliminar y si ya se gano el juego
        while (!erase_dead_countries(countryList)); //Limpia los paises muertos
        if (verify_winner(countryList) != 2) {
            isGameOver = true;
        }
    } //while
    int winner = verify_winner(countryList);
    throw_winner(winner);
}


//###############################################################################

//TODO: Implementar función del turno de player con la interfaz gráfica
void turn_player(struct DoubleLinkedList* doubleLinkedList, struct Player* player) {

    if (doubleLinkedList == NULL || doubleLinkedList->start ==NULL || player == NULL) {
        printf("ERROR2500: No se ha podido aplicar el turno del jugador");
        return;
    }
    move_player(doubleLinkedList, player);
}


//###############################################################################

//###############################################################################
void turn_corruption (struct DoubleLinkedList* doubleLinkedList) {
    random_corrupt_after_turn(doubleLinkedList);
    calculate_corruption_country_list(doubleLinkedList);
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
int verify_winner(struct DoubleLinkedList* doubleLinkedList) {

    if (doubleLinkedList == NULL || doubleLinkedList->start == NULL) {
        printf("ERROR1500: No se ha podido verificar el ganador");
        return -1;
    }
    int winner = 2;
    //Esta condición verifica si quedan 3 o menos paises vivos (Osea que ha ganado la corrupción)
    if (length_double_linked_list(doubleLinkedList) <= 3) {
        winner = 1;
        return winner; //Indica que ha ganado la corrupción
    }


    //Esta condición verifica si todos los paises vivos tienen un 0 en alguna propiedad.
    struct Country* actual = doubleLinkedList -> start;
    while (actual != NULL) {
        // Check if this country has ANY problem that is NOT zero
        if (actual->crime != 0 || actual->poverty != 0 || actual->unemployment != 0) {
            return winner; //At least one country still has problems, game continues
        }
        actual = actual -> next;

    }
    winner = 0;   //Si ha llegado hasta aquí, es porque todos los paises tienen todas las problematicas en 0
    return winner;

}

//###############################################################################

void throw_winner(int winner) {

    if (!winner) {
        printf("The player has win");
        return;
    }
    printf("The corruption has win");
}

//###############################################################################