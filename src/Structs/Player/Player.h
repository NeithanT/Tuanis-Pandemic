//
// Created by seb on 6/10/25.
//

#ifndef TUANIS_PANDEMIC_PLAYER_H
#define TUANIS_PANDEMIC_PLAYER_H
#include "../structs.h"

struct Player {
    Country* current_country;
};

void move_player(struct DoubleLinkedList* double_linked_list,struct Player* player);
void allocate_initial_player_on_map(struct DoubleLinkedList* double_linked_list, struct Player* player);

/**
 * TODO: FUNCIONES A AGREGAR:
 * Mover un jugador según el país seleccionado en la interfaz
 *
 *
 */
#endif //TUANIS_PANDEMIC_PLAYER_H

