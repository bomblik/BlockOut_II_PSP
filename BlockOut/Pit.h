/*
   File:        Pit.h
  Description: Pit management
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

#include "PolyCube.h"

#define NBTCUBE 16

class Pit
{
  public:
    Pit();

    // Initialise pit dimension
    void SetDimension(int gWidth,int gHeight,int gDepth);

    // Get dimension
    int GetWidth();
    int GetHeight();
    int GetDepth();

    // Create Pit device objects
    int Create(int scrWidth,int scrHeight,int style);

    // Return the cubeSide (3D space)
    float GetCubeSide();

    // Return the Origin (3D space)
    VERTEX GetOrigin();

    // Render the pit
    void Render(BOOL renderCube,BOOL zBuffer);

    // Render the pit level
    void RenderLevel();

#ifdef PLATFORM_PSP
    int GetLevel();
#endif

    // Release device objects
    void InvalidateDeviceObjects();
    
    // Clear the pit
    void Clear();
    
    // Get the value at the specified coordinates
    // Return 1 when (x,y,z) if out of the pit
    int GetValue(int x,int y,int z);

    // Get the value at the specified coordinates
    // Return 0 when (x,y,z) if out of the pit
    int GetValue2(int x,int y,int z);

    // Add a value at the specified coordinates
    void AddCube(int x,int y,int z);

    // Get "out of bounds" values
    void GetOutOfBounds(int x,int y,int z,int *ox,int *oy,int *oz);

    // Remove full line
    int RemoveLines();

    // Return true if the pit is empty
    BOOL IsEmpty();

  private:
    
    int CreateGrid();
    int CreateBack();
    int CreateFillingCube();
    int CreatePitLevel(int scrWidth,int scrHeight);
    GLuint CreateTexturedFillingCube(int i);
    void SetValue(int x,int y,int z,int value);
    void ChooseMapping(int i,float *sx,float *sy,float *ex,float *ey);
    GLMATERIAL *GetMaterial(int level);
    void RenderCube(int x,int y,int z);
    void RenderEdge(int x,int y,int z,int edge);
    void InitOrderMatrix();
    BOOL IsVisible(int x,int y,int z);
    BOOL IsVisible2(int x,int y,int z);
    BOOL IsLineFull(int z);
    BOOL IsLineEmpty(int z);
    void RemoveLine(int idx);
    void DrawPitLevelCubes(int x,int y,int w,int h,GLMATERIAL *mat);
    void SetPix(int x,int y,GLMATERIAL *mat);

    BOOL inited;
    GLuint gridList;
    GLuint cubeList;
    GLuint cubeTList[NBTCUBE];
    GLuint lcubeList[12];
    GLuint backList;
    GLuint sideList;
    GLuint hGridList;
    int pitLevelX;
    int pitLevelY;
    int pitLevelW;
    int pitLevelH;
    GLfloat *pitLevelSurf;
    int width;
    int height;
    int depth;
    int mSize;
    int tcubeIdx;
    float cubeSide;
    int style;
    GLMATERIAL gridMaterial;
    GLMATERIAL blackMaterial;
    GLMATERIAL darkMaterial;
    GLMATERIAL backMaterial;
    GLMATERIAL backTexMaterial;
    GLMATERIAL whiteMaterial;
    VERTEX origin;
    int *matrix;
    BLOCKITEM *orderMatrix;
    float fWidth;
    float fHeight;
    float fDepth;
    GLuint backTexture;
    GLuint cubeTexture;

};
