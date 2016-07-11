/*
  File:        BotPlayer.cpp
  Description: AI player (Evaluation function)
  Program:     BlockOut
  Author:      Jean-Luc PONS, Lieven

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
*/
#include "BotPlayer.h"
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GET_DISTANCE(x,y,z) sqrtf((float)(((x)-W)*((x)-W)+(y)*(y)+(z)*(z)));
#define EDGE_MATRIX(x,y,z)  edgeMatrix[(x) + (y)*width + (z)*area]

// --------------------------------------------------

inline int BotPlayer::GetDepthAt(int x,int y) {

  int *lMatrix = matrix + (y*width + x);
  int k;
  
  for(k=0;k<depth && (*lMatrix == 0);k++) 
    lMatrix += area;

  return k;

}

// ------------------------------------------------------------------------

float BotPlayer::CheckDeathZone() {

  float note = 0.0f;
  int W = width-1;
  int H = height-1;

  // First line
  for(int j=0;j<height;j++) {
    for(int i=0;i<width;i++) {
      if(GET_VALUE(i,j,0)) note -= 2.5f;
    }
  }

  // Death zone (L0)
  if(GET_VALUE(W,0,0))   note -= 25.0f;
  if(GET_VALUE(W,1,0))   note -= 25.0f;
  if(GET_VALUE(W-1,0,0)) note -= 25.0f;
  if(GET_VALUE(W-1,1,0)) note -= 25.0f;
  if(GET_VALUE(W-2,0,0)) note -= 25.0f;

  // Death zone (L1)
  if(GET_VALUE(W,0,1))   note -= 5.0f;
  if(GET_VALUE(W,1,1))   note -= 5.0f;
  if(GET_VALUE(W-1,0,1)) note -= 5.0f;
  if(GET_VALUE(W-1,1,1)) note -= 5.0f;

  return note;

}

// ------------------------------------------------------------------------

float  BotPlayer::GetPitNote() {

  // Compute pit note for the played block
  float note = 0.0f;
  for(int i=0;i<nbCube;i++) {    
    int x = transCube[i].x;
    int y = transCube[i].y;
    int z = transCube[i].z;
    note+=EDGE_MATRIX(x,y,z);
  }
  return note/(float)nbCube;

}

// ------------------------------------------------------------------------

void BotPlayer::CountEdge(int x,int y,int z,int *common,int *edge) {

  //v=0 => Free cell
  //v=1 => Occupied cell
  //v=2 => Cell occupied by a cube of the currently played block
  int v = GetValue(x,y,z);

  if( v==0 ) { 
    *edge = *edge + 1; 
  } else if( v==1 ) {
    *edge = *edge + 1; 
    *common = *common + 1;
  }

}

float BotPlayer::GetCommonEdge() {

  // Puzzle solver
  // Count number of common edges (facets) between the played block, the pit surface and
  // pit sides. This greatly increase the building efficiency (especialy in OOC)
  int nbCommon = 0;
  int nbEdge   = 0;

  for(int i=0;i<nbCube;i++) {
    
    int x = transCube[i].x;
    int y = transCube[i].y;
    int z = transCube[i].z;
    CountEdge(x-1,y,z,&nbCommon,&nbEdge);
    CountEdge(x+1,y,z,&nbCommon,&nbEdge);
    CountEdge(x,y-1,z,&nbCommon,&nbEdge);
    CountEdge(x,y+1,z,&nbCommon,&nbEdge);
    CountEdge(x,y,z-1,&nbCommon,&nbEdge);
    CountEdge(x,y,z+1,&nbCommon,&nbEdge);

  }

  return (float)nbCommon/(float)nbEdge;

}

// --------------------------------------------------

inline float BotPlayer::Smoothness() {

 int total=0;
 int d;

 for(int i=0;i<width;i++) {
   for(int j=0;j<height;j++) {
     dd[i][j]=GetDepthAt(i,j);
   }
 }

 for(int i=0;i<width;i++) {
   for(int j=0;j<height;j++) {
     if (i-1>=0)    {d=dd[i][j]-dd[i-1][j];total+=d*d;};
     if (j-1>=0)    {d=dd[i][j]-dd[i][j-1];total+=d*d;};
     if (j+1<height){d=dd[i][j]-dd[i][j+1];total+=d*d;};
     if (i+1<width) {d=dd[i][j]-dd[i+1][j];total+=d*d;};
   }
 }

 // Bonus on flat pit
 if(total==0) total = -10;

 return (float)total/(float)area;
 
}

// --------------------------------------------------

inline float BotPlayer::Peakness(int bias) {

 float note=0.0f;
 int   W = width-1;
 int   H = height-1;
 int   ddij;

 // Detect "1 cube size pit"
 // Note: Smothness() must be called before
 
 for(int i=0;i<width;i++) {
   for(int j=0;j<height;j++) {
     ddij = dd[i][j];
     if( i==0 && j==0 ) {
       if( (dd[i+1][j] - ddij)<=bias &&
           (dd[i][j+1] - ddij)<=bias ) note +=1.0f;
     } else if (i==0 && j==H) {
       if( (dd[i+1][j] - ddij)<=bias &&
           (dd[i][j-1] - ddij)<=bias ) note +=1.0f;
     } else if (i==W && j==0) {
       if( (dd[i-1][j] - ddij)<=bias &&
           (dd[i][j+1] - ddij)<=bias ) note +=1.0f;
     } else if (i==W && j==H) {
       if( (dd[i-1][j] - ddij)<=bias &&
           (dd[i][j-1] - ddij)<=bias ) note +=1.0f;
     } else {
       if( (dd[i-1][j] - ddij)<=bias &&
           (dd[i+1][j] - ddij)<=bias &&
           (dd[i][j-1] - ddij)<=bias &&
           (dd[i][j+1] - ddij)<=bias ) note +=1.0f;
     }
   }
 }

 return note;

}

// ------------------------------------------------------------------------
// Pit edges bonus
// ------------------------------------------------------------------------
void BotPlayer::InitPitCoef() {

  // 1 bonus coeficiens per pit cell

  // Reset all coeficients to zero
  memset(edgeMatrix,0,mSize*sizeof(float));
  int W = width-1;
  int H = height-1;

  // Fill up edge/corner bonus 
  for(int k=0;k<depth;k++) {

    for(int i=0;i<width;i++) {
      EDGE_MATRIX(i,0,k) = edgeCoef;
      EDGE_MATRIX(i,H,k) = edgeCoef;
    }
    for(int j=1;j<H;j++) {
      EDGE_MATRIX(0,j,k) = edgeCoef;
      EDGE_MATRIX(W,j,k) = edgeCoef;
    }
    // Corner
    EDGE_MATRIX(0,0,k) = cornerCoef;
    EDGE_MATRIX(0,H,k) = cornerCoef;
    EDGE_MATRIX(W,0,k) = cornerCoef;
    EDGE_MATRIX(W,H,k) = cornerCoef;

  }

  // Fill up origin to edge distance (for asymetric edges)
  // Prefere to place the block at the opposite corner
  // in case of equlity. That's the reason of the low 
  // value of edgeDistCoef.
  // However, it seems that in OOC, it is preferable to
  // go to the opposite side (not corner).

  switch(blockSet) {

    case BLOCKSET_EXTENDED:
      for(int k=0;k<depth;k++) {
        for(int j=0;j<height;j++) {
          for(int i=0;i<width;i++) {
            EDGE_MATRIX(i,j,k) += distCoef * GET_DISTANCE(i,0,0);
          }
        }
      }
      break;

    case BLOCKSET_BASIC:
    case BLOCKSET_FLAT:
      for(int k=0;k<depth;k++) {
        for(int j=0;j<height;j++) {
          for(int i=0;i<width;i++) {
            EDGE_MATRIX(i,j,k) += distCoef * GET_DISTANCE(i,j,k);
          }
        }
      }
      break;

  }

}

void BotPlayer::InitCoef() {


  // Blockset independent coefficients

  // Per block
  puzzleCoef   = 11.7f;     // Puzzle solver (per block)
  distCoef     = 0.001f;    // Distance to origin

  // Per pit
  linesCoef    =  0.7f;     // Line (layer)
  smoothCoef   = -0.28f;    // Pit surface smoothness (sqr)

  // Blockset dependent coefficients
  // Ref are given in number of cubes.
  switch(blockSet) {

  case BLOCKSET_FLAT:

    //Ref:3x3x6 FLAT Avg=29228.7 Min=109(410276037) Max=237501 nbGame=1000
    
    cornerCoef   =  0.0f;     // Pit corner occupation (per block)
    edgeCoef     =  0.0f;     // Pit side occupation (per block)
    holeCoef     = -1.9f;     // Hole
    peakCoef     = -0.0f;     // Pit surface "peakness" (peak detection)
    break;

  case BLOCKSET_EXTENDED:

    //Ref:5x5x10 EXTENDED Avg=243031 Min=272(1896058267) Max=1924054 nbGame=1000
    //Ref:3x3x18 EXTENDED Avg=488.657 Min=165(362445833) Max=1458 nbGame=1000

    // Special peak/corner/edge handling for 3x3
    if( width==3 && height==3 ) {
      // Per block
      cornerCoef   =  2.8f;     // Pit corner occupation
      edgeCoef     =  0.8f;     // Pit side occupation
      // Per pit
      peakCoef     = -0.8f;     // Pit surface "peakness" (peak detection)
    } else {
      // Per block
      cornerCoef   =  0.0f;     // Pit corner occupation
      edgeCoef     =  0.0f;     // Pit side occupation
      // Per pit
      peakCoef     = -0.0f;     // Pit surface "peakness" (peak detection)
    }
    
    // Per pit
    holeCoef     = -1.9f;     // Hole
    break;

  case BLOCKSET_BASIC:

    //Ref:3x3x6 BASIC Avg=1834.73 Min=32(1592640469) Max=15939 nbGame=10000
    
    // Per block
    cornerCoef   =  2.8f;     // Pit corner occupation
    edgeCoef     =  0.8f;     // Pit side occupation

    // Per pit
    holeCoef     = -1.1f;     // Hole
    peakCoef     = -0.81f;    // Pit surface "peakness" (peak detection)
    break;

  }

  InitPitCoef();

}

// --------------------------------------------------
// JL and Lieven's Evaluation function ;)
// --------------------------------------------------
#if 1
float BotPlayer::Evaluate() {

  DropBlock();                        // Drop the block  
  AddBlock();                         // Add the block to the pit  
  float commonEdge = GetCommonEdge(); // Puzzle solver
  float pitBonus = GetPitNote();      // Pit note
  float nbLines = RemoveLines();      // Remove lines
  float dNote = CheckDeathZone();     // Death zone
  pitBonus += dNote;
     
  float linesNote     = linesCoef  * nbLines;          // Line (layer)
  float pitNote       = pitBonus;                      // Edge,corner,distance,death zone
  float smooth2Note   = smoothCoef * Smoothness();     // Pit surface smoothness
  float commonNote    = puzzleCoef  * commonEdge;      // Puzzle solver
  float nbHoleNote    = holeCoef  * GetNbHole();       // Hole
  float peakNote      = peakCoef * Peakness(-2);       // Pit surface "peakness" (peak detection)
  
  float coveredNote   = 0.0f;  //-0.02f  * GetNbCoveredHole(); // Covered hole

  float totalNote=pitNote+smooth2Note+peakNote+commonNote+linesNote+nbHoleNote+coveredNote;

#ifdef _DEBUG
//  sprintf(infoStr,
//  "Pt=%.3f Cm=%.2f Pk=%.2f Hl=%.2f Ln=%.2f Cv=%.2f Sh=%.2f N=%.3f",
//  pitNote,commonNote,peakNote,nbHoleNote,linesNote,coveredNote,smooth2Note,totalNote);
  sprintf(infoStr,
  "Pt=%.3f Cm=%.2f Pk=%.2f Hl=%.2f Ln=%.2f Sh=%.2f N=%.3f",
  pitNote,commonNote,peakNote,nbHoleNote,linesNote,smooth2Note,totalNote);
#endif

  return totalNote;
}
#endif

// ----------------------------------------------------------------------
// Lieven's original evaluation function (already quite good in 3DMania)
// ----------------------------------------------------------------------
#if 0
float BotPlayer::Evaluate() {

  DropBlock(); // Drop the block
  AddBlock(); // Add the block to the pit
  float nbLines = RemoveLines();   // Remove lines

  float thick=(float)nbCubeInPit-nbLines*(float)area; //Number of blocks
  float nbHole1 = GetNbHole(); // Number of holes
  float smooth2 = Smoothness2(); // How smooth is the pitch (sqr)
  //float smooth = Smoothness1(); // How smooth is the pitch (abs)
  float edgel = Edges(); //if you have to go high, go high on the edge
  
  //float note = -27*thick-75.0f*nbHole1-3.0f*smooth2+2*edgel;        //FLAT FUN 
  float note = -27.0f*thick-75.0f*nbHole1-3.0f*smooth2+17.0f*edgel; //3DMANIA
  //return -17.0f*thick-500.0f*nbHole1-40.0f*smooth+11.0f*edgel;  //OUT OF CONTROL

#ifdef _DEBUG
  sprintf(infoStr,
  "thick=%.3f, edge1=%.3f, smooth=%.3f, hole=%.0f\nnote=%f",
  thick,edgel,smooth2,nbHole1,note);
#endif

  return note;

}

// --------------------------------------------------

inline float BotPlayer::Smoothness2() {

 int hh[7][7];
 int total=0,d;

 for(int i=0;i<width;i++) {
   for(int j=0;j<height;j++) {
    hh[i][j]=GetDepthAt(i,j);
   }
 }

 for(int i=0;i<width;i++) {
   for(int j=0;j<height;j++) {
     if (i-1>=0)    {d=hh[i][j]-hh[i-1][j];total+=d*d;}
     if (j-1>=0)    {d=hh[i][j]-hh[i][j-1];total+=d*d;}
     if (j+1<height){d=hh[i][j]-hh[i][j+1];total+=d*d;}
     if (i+1<width) {d=hh[i][j]-hh[i+1][j];total+=d*d;}
   }
 }

 // Bonus if empty pit (flush)
 int good=50;
 for(int i=0;i<width;i++) 
   for(int j=0;j<height;j++) 
    if (hh[i][j]) good=0;

  return (float)(total-good);

}

// --------------------------------------------------

inline float BotPlayer::Edges() {

  // Count cubes near the edges
  // Count 2 times cubes in corner
  int *lMatrix = matrix;
  int total = 0;
  int W = width-1;
  int H = height-1;
  for(int k=0;k<depth;k++) {

    for(int i=0;i<width;i++) {
      if( lMatrix[i+0*width] ) total++;
      if( lMatrix[i+H*width] ) total++;
    }
    for(int j=0;j<height;j++) {
      if( lMatrix[0+j*width] ) total++;
      if( lMatrix[W+j*width] ) total++;
    }

    lMatrix += area;

  }

  return (float)total;
  
}

// --------------------------------------------------

inline float BotPlayer::Smoothness1() {

 int total=0,d;

 for(int i=0;i<width;i++) {
   for(int j=0;j<height;j++) {
    dd[i][j]=GetDepthAt(i,j);
   }
 }

 for(int i=0;i<width;i++) {
   for(int j=0;j<height;j++) {
     if (i-1>=0)    {d=dd[i][j]-dd[i-1][j];total+=abs(d);}
     if (j-1>=0)    {d=dd[i][j]-dd[i][j-1];total+=abs(d);}
     if (j+1<height){d=dd[i][j]-dd[i][j+1];total+=abs(d);}
     if (i+1<width) {d=dd[i][j]-dd[i+1][j];total+=abs(d);}
   }
 }

 // Bonus if empty pit (flush)
 int good=50;
 for(int i=0;i<width;i++) 
   for(int j=0;j<height;j++) 
    if (dd[i][j]) good=0;

  return (float)(total-good);

}

#endif
