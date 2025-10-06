#include "random.h"

int initialCorruption(struct DoubleLinkedList* list) {
    //TODO colocar el srand(time(NULL)); en main
    int modifiedCount = 0;
    for (int i = 0; i < 9; i++) {
        int modifier = rand() % 21;
        int higher = rand() % 2;
        modifiedAspectsCountries(list,modifier,modifiedCount,higher);
        modifiedCount++;
    }
    return 0;
}

int modifiedAspectsCountries(struct DoubleLinkedList* list, int position, int modifiedCount, int higher) {
    struct Country *actual = list->start;
    int current = 0;
    while (current != position) {
        actual = actual->next;
        current++;
    }
    while (actual -> poorness != 0) {
        if (actual -> next != NULL)
            actual = actual -> next;
        else break;
    }
    while (actual -> poorness != 0) {
        if (actual -> prev != NULL) {
            actual = actual -> prev;
        }
        else break;
    }
    if (modifiedCount < 3) {
        actual->poorness = (higher == 0) ? 3 : 2;
        actual->gangs = (higher == 1) ? 3 : 2;
        return 0;
    }
    if (modifiedCount >= 3 && modifiedCount < 6) {
        actual->poorness = (higher == 0) ? 2 : 1;
        actual->gangs = (higher == 0) ? 1 : 2;
        return 0;
    }
    actual->poorness = 1;
    actual->gangs = 1;
    return 0;
}
