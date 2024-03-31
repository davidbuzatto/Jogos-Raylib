#include <stdbool.h>

#ifndef TETROMINOES_MACROS
  #define TT true
  #define FF false
  #define TETROMINOES_MACROS
#endif

bool tetrominoeH[4][4][4] = {

    { { FF, FF, FF, FF },
      { TT, TT, TT, TT },
      { FF, FF, FF, FF },
      { FF, FF, FF, FF } },

    { { FF, FF, TT, FF },
      { FF, FF, TT, FF },
      { FF, FF, TT, FF },
      { FF, FF, TT, FF } },

    { { FF, FF, FF, FF },
      { FF, FF, FF, FF },
      { TT, TT, TT, TT },
      { FF, FF, FF, FF } },

    { { FF, TT, FF, FF },
      { FF, TT, FF, FF },
      { FF, TT, FF, FF },
      { FF, TT, FF, FF } }

};

// { width, height, columnOffset, lineOffset }
int tetrominoeHDimensions[4][4] = {
    { 4, 1, 0, 1 },
    { 1, 4, 2, 0 },
    { 4, 1, 0, 2 },
    { 1, 4, 1, 0 }
};