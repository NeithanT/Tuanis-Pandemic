//
// Created by seb on 6/10/25.
//

#ifndef TUANIS_PANDEMIC_PLAYER_H
#define TUANIS_PANDEMIC_PLAYER_H
#include "../structs.h"

struct Player {
    struct Country* current_country;
};

void move_player(struct DoubleLinkedList* doubleLinkedList,struct Player* player);

struct Country* pick_country();

struct Player* allocate_initial_player_on_map(struct DoubleLinkedList* doubleLinkedList);

/**
 * TODO: FUNCIONES A AGREGAR:
 * Mover un jugador según el país seleccionado en la interfaz
 *
 *
 */
#endif //TUANIS_PANDEMIC_PLAYER_H

