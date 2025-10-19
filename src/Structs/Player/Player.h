//
// Created by seb on 6/10/25.
//

#ifndef TUANIS_PANDEMIC_PLAYER_H
#define TUANIS_PANDEMIC_PLAYER_H
#include "../structs.h"

struct Player {
    struct Country* current_country;
};

/**
 * Mueve al jugador a un país seleccionado, actualizando su posición en la lista
 * doubleLinkedList Puntero a la lista de países
 * player Puntero al jugador a mover
 */
void move_player(struct DoubleLinkedList* doubleLinkedList,struct Player* player);

/**
 * Permite al jugador seleccionar un país para moverse
 * Puntero al país seleccionado
 */
struct Country* pick_country();

/**
 * Asigna al jugador inicial en un país aleatorio o predeterminado del mapa
 * doubleLinkedList Puntero a la lista de países
 * Puntero al jugador inicializado
 */
struct Player* allocate_initial_player_on_map(struct DoubleLinkedList* doubleLinkedList);

/**
 * TODO: FUNCIONES A AGREGAR:
 * Mover un jugador según el país seleccionado en la interfaz
 *
 *
 */
#endif //TUANIS_PANDEMIC_PLAYER_H

