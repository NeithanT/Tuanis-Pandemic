#ifndef SRC_RANDOM_H
#define SRC_RANDOM_H
#include "<stdio.h>"
#include "<stdlib.h>"
#include "../structs.h"

int modifiedAspectsCountries(struct DoubleLinkedList* list, int position, int modifiedCount, int higher);

int initialCorruption(struct DoubleLinkedList* list);

void modifiedAfterTurn(struct DoubleLinkedList* list, int position, int change);

void AfterTurn(struct DoubleLinkedList* list);

#endif //SRC_RANDOM_H