#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "include/raylib.h"

double toRadians( double degrees ) {
    return degrees * PI / 180.0;
}

double toDegrees( double radians ) {
    return radians * 180.0 / PI;
}

bool colorEquals( Color c1, Color c2 ) {
    return c1.r == c2.r &&
         c1.g == c2.g &&
         c1.b == c2.b;
}