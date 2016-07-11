/*
  File:        BotPlayer.h
  Description: AI player
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

#ifndef BOTPLAYERH
#define BOTPLAYERH

#include "Pit.h"
#include "BotMatrix.h"

#define MAX_MOVE 256

typedef struct {

  BLOCKITEM m[5];
  int r0;
  int r1;
  int r2;
  int nbCube;

} Cube;

typedef struct {

  Cube c[24];
  int  nbTransform;

} TransformSet;

#define GET_VALUE(x,y,z) matrix[(x) + (y)*width + (z)*area]

class BotPlayer
{
  public:
    BotPlayer();

    // Initialise the bot player
    void Init(int w,int h,int d,int blockSet);

    // Compute the best moves for the given block
    void GetMoves(Pit *pit,PolyCube *p,int x,int y,int z,AI_MOVE *moves,int *nbMove);

    // Return eval info
    char *GetInfo(Pit *pit,BLOCKITEM *cubes,int nbCube);

    // Evaluation coefficients
    float linesCoef;
    float holeCoef;
    float smoothCoef;
    float puzzleCoef;
    float edgeCoef;
    float cornerCoef;
    float peakCoef;
    float distCoef;

  private:

    // Pit and transformation model
    void      AnalysePlane(Pit *pit,int sX,int sY,int eX,int eY);
    void      RotateBlock(int rotation);
    void      TransformCube();
    void      TranslateCube(int tx,int ty,int tz);
    BOOL      IsValidPos();
    void      GetBounds(int *xmin,int *ymin,int *zmin,int *xmax,int *ymax,int *zmax);
    int       GetValue(int x,int y,int z);
    void      SetValue(int x,int y,int z,int value);
    int       GetDelta(int x);
    void      AddBlock();
    BOOL      IsLineFull(int z);
    void      RemoveLine(int idx);
    void      RestorePit();
    void      CopyPit(Pit *pit);
    int       GetNbCubeInPit();

    // Evaluation 
    float     Evaluate();         // Main evaluation function
    void      DropBlock();        // Drop the block
    float     RemoveLines();      // Remove lines
    float     GetNbHole();        // Total number of holes
    float     GetNbCoveredHole(); // Hole cover
    int       GetFreeDepth();     // 0 to depth
    float     GetCommonEdge();    // 0 to 1    
    void      InitCoef();         // Initialise AI coeficients
    void      CountEdge(int x,int y,int z,int *common,int *edge);
    float     Smoothness();
    int       GetDepthAt(int x,int y);
    void      InitPitCoef();
    float     GetPitNote();
    float     Peakness(int bias);
    float     CheckDeathZone();

    float     Smoothness1();  // Original Lieven's methods (not used)
    float     Smoothness2();  // Original Lieven's methods (not used)
    float     Edges();        // Original Lieven's methods (not used)

    // Global
    PolyCube   *block;

    // Block position
    BotMatrix   matBlock;
    int         xBlock;
    int         yBlock;
    int         zBlock;
    BLOCKITEM   transCube[5];
    BLOCKITEM   transCubeOrg[5];
    int         nbCube;

    // Analysed paths
    AI_MOVE curPath[MAX_MOVE];  // Current path
    int     nbCurPath;          // Current path

    AI_MOVE bestPath[MAX_MOVE];  // Best path
    int     nbBestPath;          // Best path
    float   bestNote;            // Best path note
    BLOCKITEM rotCenter;         // Rotation center
    int     freeDepth;           // Free depth at the top of the pit
    int     nbCubeInPit;         // Number of cube within the pit
    char    infoStr[4096];       // Debug info
    float  *edgeMatrix;          // Pit edge bonus/malus
    int     blockSet;            // The block set

    // Pit
    int width;
    int height;
    int depth;
    int area;
    int mSize;
    int *matrix;
    int *matrixOrg;
    int  dd[MAX_PITWIDTH][MAX_PITHEIGHT];

#ifdef AI_TEST

    PolyCube allPolyCube[NB_POLYCUBE]; // All polycubes (41 items)
    int      possible[NB_POLYCUBE];    // All possible polycubes for the setup
    int      nbPossible;               // Number of possible polycube
    int      pIdx;                     // Current polycube index (in the allPolyCube array)
    int      nbPlayedCube;             // Number of played cube

    void     InitPolyCube();
    void     SelectPolyCube();
    int      Move(Pit *pit);

    void     AddTransform(int idx,int r0,int r1,int r2);
    BOOL     FindCube(int x,int y,int z,Cube *c);
    TransformSet *allTransform;

public:

    int   Test(int limit,int *Seed=0);
    void  AnalyseSymmetry();


#endif

};

#endif /* BOTPLAYERH */
