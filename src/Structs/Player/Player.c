//
// Created by seb on 6/10/25.
//
//

#include "Player.h"
#include <stdio.h>
#include <stdlib.h>

struct Country* pick_country() {
    return NULL; // TODO: implementar con interfaz
}

//###############################################################################

struct Player* allocate_initial_player_on_map(struct DoubleLinkedList* doubleLinkedList) {

    if (doubleLinkedList == NULL || doubleLinkedList->start == NULL) {
        printf("ERROR2100: No se ha podido colocar al jugador en el mapa");
        return NULL;
    }

    struct Player* newPlayer = calloc(1, sizeof(struct Player));
    struct Country* current_country = doubleLinkedList->start;
    newPlayer->current_country = current_country;

    int sizeList = length_double_linked_list(doubleLinkedList);
    int randomPosition = rand() % sizeList;

    while (randomPosition-- > 0) {
        current_country = current_country->next;
        newPlayer->current_country = current_country;
    }
    return newPlayer;
}


//###############################################################################

void move_player(struct DoubleLinkedList* doubleLinkedList,struct Player* player) {

    if ( doubleLinkedList == NULL || doubleLinkedList->start == NULL || player == NULL) {
        printf("Cant move the player because the parameters are invalid");
        return;
    }
    struct Country* selected_country = pick_country();

    while (selected_country == NULL) {
        printf("Invalid Country selected");
        selected_country = pick_country();
    }
    player->current_country = selected_country; //TODO Esta parte es la que debe conectarse con la interfaz
    printf("%s%s", "The player has moved to: ", player->current_country->name);
}

//###############################################################################