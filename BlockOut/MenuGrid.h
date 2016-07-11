/*
   File:        MenuGrid.h
  Description: Menu graphics management
  Program:     BlockOut
  Author:      Jean-Luc PONS

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
*/

#include <math.h>
#include "GLApp/GLSprite.h"
#include "GLApp/GLMatrix.h"
#include "Types.h"

#define GRID_WIDTH   70
#define GRID_HEIGHT  50

class MenuGrid {

  public:
    MenuGrid();

    // Initialise device objects
    int Create();

    // Render the grid
    void Render();

    // Release device objects
    void InvalidateDeviceObjects();

    // Set value
    void SetValue(int x,int y,int value);

    // Clear the grid
    void Clear();

  private:

    int CreateGrid();
    int CreateCube();
    void RenderCube(int x,int y);

    // Grid variable
    float cSide;
    float startX;
    float startY;
    float startZ;
    int   matrix[GRID_HEIGHT*GRID_WIDTH];
    POINT2D orderMatrix[GRID_HEIGHT*GRID_WIDTH];

    // Graphics
    GLuint gridList;
    GLMATERIAL gridMaterial;

    GLuint redList;
    GLMATERIAL redMaterial;

    GLuint blueList;
    GLMATERIAL blueMaterial;

    GLuint  topList;

    GLuint  nwList;
    GLuint  neList;
    GLuint  swList;
    GLuint  seList;

    GLuint  bnList;
    GLuint  bwList;
    GLuint  bsList;
    GLuint  beList;

};

