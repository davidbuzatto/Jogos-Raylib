#include <stdbool.h>

#ifndef TETROMINOES_MACROS
  #define TT true
  #define FF false
  #define TETROMINOES_MACROS
#endif

bool tetrominoeSQ[4][4][4] = {

    { { TT, TT, FF, FF },
      { TT, TT, FF, FF },
      { FF, FF, FF, FF },
      { FF, FF, FF, FF } },

    { { TT, TT, FF, FF },
      { TT, TT, FF, FF },
      { FF, FF, FF, FF },
      { FF, FF, FF, FF } },

    { { TT, TT, FF, FF },
      { TT, TT, FF, FF },
      { FF, FF, FF, FF },
      { FF, FF, FF, FF } },

    { { TT, TT, FF, FF },
      { TT, TT, FF, FF },
      { FF, FF, FF, FF },
      { FF, FF, FF, FF } }

};

// { width, height, columnOffset, lineOffset }
int tetrominoeSQDimensions[4][4] = {
    { 2, 2, 0, 0 },
    { 2, 2, 0, 0 },
    { 2, 2, 0, 0 },
    { 2, 2, 0, 0 }
};