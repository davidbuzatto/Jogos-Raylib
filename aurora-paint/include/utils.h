/**
 * @file utils.h
 * @author Prof. Dr. David Buzatto
 * @brief Utilitary function and constant declarations.
 * 
 * @copyright Copyright (c) 2024
 */
#pragma once

#include "raylib.h"

double toRadians( double degrees );
double toDegrees( double radians );
void createDummyImage( Image *img, int verticalMargin );
void loadAndPrepareImage( Image *img, const char *path, int verticalMargin );