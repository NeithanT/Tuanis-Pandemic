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
    while (actual -> next != NULL && actual -> poorness != 0) {
        actual = actual -> next;
    }

    while (actual -> prev != NULL && actual -> poorness != 0) {
        actual = actual -> prev;
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



int AfterTurn(struct DoubleLinkedList* list) {
    for (int i = 0; i < 9; i++) {
        int modifier = rand() % 21;
        int higher = rand() % 2;
        modifiedAfterTurn(list,modifier,higher);
    }
}

void modifiedAfterTurn(struct DoubleLinkedList* list, int position, int change) {
    struct Country *actual = list->start;
    int current = 0;
    while (current != position) {
        actual = actual->next;
        current++;
    }
    while (actual -> next != NULL && actual -> poorness != 0) {
        actual = actual -> next;
    }

    while (actual -> prev != NULL && actual -> poorness != 0) {
        actual = actual -> prev;
    }

    switch (change) {
        case 0:
            if (actual -> poorness == 3) {
                while (actual -> prev != NULL && actual -> prev -> poorness == 3) {

                    actual = actual -> prev;

                }
                while (actual -> next != NULL && actual -> next -> poorness == 3) {
                    actual = actual -> next;
                }
                if (actual -> prev != NULL && actual -> prev -> poorness != 3) {
                    actual -> prev -> poorness += 1;
                }
                if (actual -> next != NULL && actual -> next -> poorness != 3) {
                    actual -> next -> poorness += 1;
                }

            }

            else actual -> poorness += 1;
            break;
        case 1:
            if (actual -> gangs == 3) {

                while (actual -> prev != NULL && actual -> prev -> gangs == 3) {
                    actual = actual -> prev;
                }

                while (actual -> next != NULL && actual -> next -> gangs == 3) {
                    actual = actual -> next;
                }

                if (actual -> prev != NULL && actual -> prev -> gangs != 3)
                    actual -> prev -> gangs += 1;
                if (actual -> next != NULL && actual -> next -> gangs != 3)
                    actual -> next -> gangs += 1;

            }

            else actual -> gangs += 1;
            break;
    }

}