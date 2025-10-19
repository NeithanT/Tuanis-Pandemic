#ifndef TEC_PLAY_H
#define TEC_PLAY_H
#include <stdio.h>
#include <stdlib.h>
#include "../Structs/structs.h"
#include <stdbool.h>
#include <string.h>
#include <asm-generic/errno.h>

#include "../Structs/structs.h"
#include "../Structs/Random/random.h"
#include "../Structs/Player/Player.h"

/**
 * Inicia el juego, configurando el estado inicial
 */
void start_game();

/**
 * Maneja el turno del jugador, permitiendo movimientos y acciones
 * doubleLinkedList Puntero a la lista de países
 * player Puntero al jugador
 */
void turn_player(struct DoubleLinkedList* doubleLinkedList, struct Player* player);

/**
 * Maneja el turno de corrupción, aplicando cambios aleatorios
 * doubleLinkedList Puntero a la lista de países
 */
void turn_corruption (struct DoubleLinkedList* doubleLinkedList);

/**
 * Consulta los aspectos de los países en la lista
 * list Puntero a la lista de países
 * Código o valor de consulta
 */
int consult_aspects(struct DoubleLinkedList* list);

/**
 * Verifica si hay un ganador en el juego
 * doubleLinkedList Puntero a la lista de países
 * ID del ganador, o 0 si no hay
 */
int verify_winner(struct DoubleLinkedList* doubleLinkedList);

/**
 * Anuncia o maneja el ganador del juego
 * winner ID del ganador
 */
void throw_winner(int winner);

#endif //TEC_PLAY_H