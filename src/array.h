#ifndef ARRAY_H
#define ARRAY_H

// dynamic array where idx:
// 0 contains the capacity value
// 1 contains the size (occupied) valued
// from 2 until the end - actual values

#define ArrayPush(array, value)                             \
    do {                                                    \
        (array) = ArrayHold((array), 1, sizeof(*(array)));  \
        (array)[ArrayLength(array) - 1] = (value);          \
    } while(0);

void* ArrayHold(void* array, const int count, const int itemSize);
int ArrayLength(void* array);
void ArrayFree(void** array);

#endif
