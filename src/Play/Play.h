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



int StartGame();

void turnPlayer(struct DoubleLinkedList* doubleLinkedList);

void turnAlly(struct DoubleLinkedList* double_linked_list);

void turnCorruption (struct DoubleLinkedList* doubleLinkedList);

int consultAspects(struct DoubleLinkedList* list);

int verifyWinner(struct DoubleLinkedList* doubleLinkedList);

void throwWinner(int winner);

#endif //TEC_PLAY_H