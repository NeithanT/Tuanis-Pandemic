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
 * @brief Inicia el flujo principal del juego.
 *
 * Configura el estado inicial del tablero, los jugadores y las variables
 * necesarias antes de que comiencen los turnos.
 */
void start_game();

/**
 * @brief Maneja todas las acciones disponibles para un jugador en su turno.
 *
 * Permite al jugador realizar movimientos, interacciones o cualquier otra
 * acción definida en las reglas del juego.
 *
 * @param doubleLinkedList Puntero a la lista doblemente enlazada de
 * países que conforman el mapa.
 * @param player Puntero a la estructura del jugador cuyo turno se está manejando.
 */
void turn_player(struct DoubleLinkedList* doubleLinkedList, struct Player* player);

/**
 * @brief Aplica los efectos de la corrupción en el mapa.
 *
 * Este turno ejecuta cambios aleatorios o predefinidos en las propiedades
 * de los países, simulando un evento de corrupción global o localizado.
 *
 * @param doubleLinkedList Puntero a la lista doblemente enlazada de
 */
void turn_corruption (struct DoubleLinkedList* doubleLinkedList);

/**
 * @brief Consulta y posiblemente muestra aspectos específicos de los países.
 *
 * Permite obtener información o estadísticas de los países
 * según un código de consulta.
 *
 * @param list Puntero a la lista de países que se va a consultar.
 * @return El valor o resultado de la consulta.
 */

int consult_aspects(struct DoubleLinkedList* list);

/**
 * @brief Verifica las condiciones de victoria para determinar si un jugador ha ganado.
 *
 * Itera sobre el estado actual del juego (países y jugadores) para
 * chequear si se cumple alguna condición de finalización del juego.
 *
 * @param doubleLinkedList Puntero a la lista de países.
 * @return El ID del jugador que ha ganado.
 * @return **0** si aún no hay un ganador.
 */

int verify_winner(struct DoubleLinkedList* doubleLinkedList);

/**
 * @brief Anuncia o ejecuta la lógica de fin de juego al detectar un ganador.
 *
 * Muestra un mensaje o realiza las acciones finales asociadas a la victoria
 * de un jugador.
 *
 * @param winner El ID del jugador que ha ganado la partida.
 */
void throw_winner(int winner);

#endif //TEC_PLAY_H