#ifndef SRC_RANDOM_H
#define SRC_RANDOM_H
#include "../structs.h"

void calculate_corruption(struct Country* country);

int modify_aspects_country(struct DoubleLinkedList* list, int position, int modifiedCount, int higher);

int initial_corruption(struct DoubleLinkedList* list);

void calculate_corruption_country_list(struct DoubleLinkedList* doubleLinkedList);

void modify_aspects_after_turn(struct DoubleLinkedList* list, int position, int change);

void random_corrupt_after_turn(struct DoubleLinkedList* list);

void reduce_random_problem(struct Country* country);

#endif //SRC_RANDOM_H