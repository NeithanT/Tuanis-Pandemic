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

void start_game();

void turn_player(struct DoubleLinkedList* doubleLinkedList, struct Player* player);

void turn_corruption (struct DoubleLinkedList* doubleLinkedList);

int consult_aspects(struct DoubleLinkedList* list);

int verify_winner(struct DoubleLinkedList* doubleLinkedList);

void throw_winner(int winner);

#endif //TEC_PLAY_H