//
// types.h: useful typedefs, misc.
// -----------------------------------

#ifndef SIGEN_TYPES_H
#define SIGEN_TYPES_H

// rules to include headers necessary for uint32_t, etc.
#ifdef __linux__
#include <stdint.h>
#endif

typedef uint32_t ui32;
typedef uint16_t ui16;
typedef uint8_t  ui8;

#endif
