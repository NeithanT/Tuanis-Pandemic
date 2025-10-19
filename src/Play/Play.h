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

void StartGame();

void turnPlayer(struct DoubleLinkedList* doubleLinkedList, struct Player* player);

void turnAlly(struct Player* ally);

void turnCorruption (struct DoubleLinkedList* doubleLinkedList);

int consultAspects(struct DoubleLinkedList* list);

int verifyWinner(struct DoubleLinkedList* doubleLinkedList);

void throwWinner(int winner);

#endif //TEC_PLAY_H