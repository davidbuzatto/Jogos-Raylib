#include <stdbool.h>

#ifndef TETROMINOES_MACROS
  #define TT true
  #define FF false
  #define TETROMINOES_MACROS
#endif

bool tetrominoeIL[4][4][4] = {

    { { FF, FF, FF, FF },
      { TT, TT, TT, FF },
      { FF, FF, TT, FF },
      { FF, FF, FF, FF } },

    { { FF, TT, FF, FF },
      { FF, TT, FF, FF },
      { TT, TT, FF, FF },
      { FF, FF, FF, FF } },

    { { TT, FF, FF, FF },
      { TT, TT, TT, FF },
      { FF, FF, FF, FF },
      { FF, FF, FF, FF } },

    { { FF, TT, TT, FF },
      { FF, TT, FF, FF },
      { FF, TT, FF, FF },
      { FF, FF, FF, FF } },

};

// { width, height, columnOffset, lineOffset }
int tetrominoeILDimensions[4][4] = {
    { 3, 2, 0, 1 },
    { 2, 3, 0, 0 },
    { 3, 2, 0, 0 },
    { 2, 3, 1, 0 }
};