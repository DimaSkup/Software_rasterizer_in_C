// ==================================================================
// Filename:    swap.h
// Description: just swap functions
// Created:     05.02.25
// ==================================================================
#ifndef SWAP_H
#define SWAP_H

// integer swap (fast way)
#define SWAPI(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

// float swap
void SWAPF(float* a, float* b);

#endif
