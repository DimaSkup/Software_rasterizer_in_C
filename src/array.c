#include <stdio.h>
#include <stdlib.h>
#include "array.h"

#define ARRAY_RAW_DATA(array) ((int*)(array) - 2)
#define ARRAY_CAPACITY(array) (ARRAY_RAW_DATA(array)[0])
#define ARRAY_OCCUPIED(array) (ARRAY_RAW_DATA(array)[1])

void* ArrayHold(void* array, const int count, const int itemSize)
{
    if (array == NULL)
    {
        int rawSize = (sizeof(int) * 2) + (itemSize * count);
        int* base = (int*)malloc(rawSize);
        base[0] = count;        // capacity
        base[1] = count;        // occupied
        return base + 2;
    }
    else if (ARRAY_OCCUPIED(array) + count <= ARRAY_CAPACITY(array))
    {
        ARRAY_OCCUPIED(array) += count;
        return array;
    }
    else
    {
        int neededSize = ARRAY_OCCUPIED(array) + count;
        int doubleCurr = ARRAY_CAPACITY(array) * 2;
        int capacity = (neededSize > doubleCurr) ? neededSize : doubleCurr;
        int occupied = neededSize;
        int rawSize = sizeof(int) * 2 + itemSize * capacity;
        int* base = (int*)realloc(ARRAY_RAW_DATA(array), rawSize);
        base[0] = capacity;
        base[1] = occupied;
        return base + 2;
    }
}

///////////////////////////////////////////////////////////

int ArrayLength(void* array)
{
    return (array != NULL) ? ARRAY_OCCUPIED(array) : 0;
}

///////////////////////////////////////////////////////////

void ArrayFree(void** array)
{
    if (array != NULL)
    {
        void* ptr = ARRAY_RAW_DATA(*array);
        free(ptr);
        *array = NULL;
    }
}
