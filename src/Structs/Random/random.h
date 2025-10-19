#ifndef SRC_RANDOM_H
#define SRC_RANDOM_H
#include "../structs.h"

void calculateCorruption(struct Country* country);

int modifyAspectsCountry(struct DoubleLinkedList* list, int position, int modifiedCount, int higher);

int initialCorruption(struct DoubleLinkedList* list);

void calculateCorruptionCountryList(struct DoubleLinkedList* doubleLinkedList);

void modifyAspectsAfterTurn(struct DoubleLinkedList* list, int position, int change);

void randomCorruptAfterTurn(struct DoubleLinkedList* list);

void reduceRandomProblem(struct Country* country);

#endif //SRC_RANDOM_H