//
// Created by seb on 6/10/25.
//
//

#include "Player.h"

//###############################################################################

void allocate_initial_player_on_map(struct DoubleLinkedList* double_linked_list, struct Player* player) {

    int size = lengthDoubleLinkedList(double_linked_list);
    int random_position = rand() % size;
    struct Country* current_country = double_linked_list->start;
    player->current_country = current_country;
    //TODO: LA VERDAD AQUI NO SE SI ES > O >=, SEGUN MIS DIAGRAMAS ES >
    while (random_position-- > 0) {
        player->current_country = current_country;
        current_country = current_country->next;
    }
}


//###############################################################################

/*
void move_player(struct DoubleLinkedList* double_linked_list,struct Player* player) {

    if ( double_linked_list == NULL || double_linked_list->start == NULL || player == NULL) {
        printf("Cant move the player because the parameters are invalid");
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


