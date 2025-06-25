#include "color.h"
#include "macros.h"
#include <stddef.h>


USHORT RGB16Bit565(int r, int g, int b)
{
    // builds a 5.6.5 format 16 bit pixel 
    // assumes input is RGB 0-255 each channel
    r >>= 3; 
    g >>= 2; 
    b >>= 3;
    return RGB16BIT565((r),(g),(b));
}

///////////////////////////////////////////////////////////

USHORT RGB16Bit555(int r, int g, int b)
{
    // builds a 5.5.5 format 16 bit pixel (1 bit for alpha channel)
    // assumes input is RGB 0-255 each channel
    r >>= 3;
    g >>= 3;
    b >>= 3;
    return RGB16BIT555((r),(g),(b));
}

// ==================================================================

// set function pointer to something
USHORT (*RGB16Bit)(int, int, int) = &RGB16Bit565;
