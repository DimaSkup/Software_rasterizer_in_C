#include "common_types.h"

// function ptr to RGB16 builder
extern USHORT (*RGB16Bit)(int r, int g, int b);


USHORT RGB16Bit565(int r, int g, int b);
USHORT RGB16Bit555(int r, int g, int b);

