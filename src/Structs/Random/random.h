#ifndef SRC_RANDOM_H
#define SRC_RANDOM_H
#include "../structs.h"



float calculateCorruption(struct Country* country);

int modifyAspectsCountry(struct DoubleLinkedList* list, int position, int modifiedCount, int higher);

int initialCorruption(struct DoubleLinkedList* list);

void calculateCorruptionCountryList(struct DoubleLinkedList* doubleLinkedList);

void modifyAspectsAfterTurn(struct DoubleLinkedList* list, int position, int change);

void randomCorruptAfterTurn(struct DoubleLinkedList* list);



#endif //SRC_RANDOM_H