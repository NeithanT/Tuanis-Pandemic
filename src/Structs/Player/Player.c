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

struct Player* allocateInitialPlayerOnMap(struct DoubleLinkedList* doubleLinkedList) {

    if (doubleLinkedList == NULL || doubleLinkedList->start == NULL) {
        printf("ERROR2100: No se ha podido colocar al jugador en el mapa");
        return NULL;
    }

    struct Player* newPlayer = calloc(1, sizeof(struct Player));
    struct Country* current_country = doubleLinkedList->start;
    newPlayer->current_country = current_country;

    int sizeList = lengthDoubleLinkedList(doubleLinkedList);
    int randomPosition = rand() % sizeList;

    while (randomPosition-- > 0) {
        current_country = current_country->next;
        newPlayer->current_country = current_country;
    }
    return newPlayer;
}


//###############################################################################

void move_player(struct DoubleLinkedList* doubleLinkedList,struct Player* player, int isAlly) {

    if ( doubleLinkedList == NULL || doubleLinkedList->start == NULL || player == NULL) {
        printf("Cant move the player because the parameters are invalid");
    }
    if (isAlly) {
        moveAllyRandomCountry(player);
    }
    else {
    struct Country* selected_country = pick_country();

    while (selected_country == NULL) {
        printf("Invalid Country selected");
        selected_country = pick_country();
    }
    player->current_country = selected_country; //TODO Esta parte es la que debe conectarse con la interfaz
    printf("%s%s", "The player has moved to: ", player->current_country->name);
}
}

//###############################################################################

void moveAllyRandomCountry(struct Player* player) {

    if (!player || !player->current_country) {
        printf("ERROR2500: No se ha podido obtener un Country Random");
        return;
    }

    // Use connected_countries list for realistic movement
    if (player->current_country->connected_countries && 
        player->current_country->connected_countries->connected_count > 0) {
        
        // Count available connected countries
        int count = player->current_country->connected_countries->connected_count;
        
        if (count > 0) {
            // Pick a random connected country
            int random_choice = rand() % count;
            player->current_country = player->current_country->connected_countries->connected_list[random_choice];
            return;
        }
    }
    
    // Fallback to old behavior if no connections found
    int randomMove = rand() % 2;
    if (randomMove == 0) {
        if (player->current_country->prev != NULL) {
            player->current_country = player->current_country->prev;
        } else if (player->current_country->next != NULL) {
            player->current_country = player->current_country->next;
        }
    } else {
        if (player->current_country->next != NULL) {
            player->current_country = player->current_country->next;
        } else if (player->current_country->prev != NULL) {
            player->current_country = player->current_country->prev;
        }
    }
}