#pragma once

#include <stdbool.h>

#include "Types.h"
#include "raylib.h"

void drawBlock( Block *block );
BoundingBox getBlockBoundingBox( Block *block );
