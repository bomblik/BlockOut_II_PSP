/*
  File:        Types.h 
  Description: Various basic types definitions
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

#include "GLApp/GLApp.h"

#ifndef TYPESH
#define TYPESH

#define STR(x) ((char *)x)

#ifdef WINDOWS

typedef unsigned __int32 uint32;
typedef __int32 int32;

#else

typedef unsigned int uint32;
typedef int int32;

#endif

//-----------------------------------------------------------------------------
// Global definitions
//-----------------------------------------------------------------------------

#define PI                 3.1415926535f
#define STARTZ             0.87f
#define FAR_DISTANCE       10.0f
#define MAX_CUBE           50
#define NB_POLYCUBE        41

// Vertex format
#define VERTEX_FORMAT      (D3DFVF_XYZ)
#define FACEVERTEX_FORMAT  (D3DFVF_XYZ | D3DFVF_NORMAL)
#define TFACEVERTEX_FORMAT  (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

// Max pit dimension
#define MAX_PITWIDTH       7
#define MAX_PITHEIGHT      7
#define MAX_PITDEPTH       18

// Min pit dimension
#define MIN_PITWIDTH       3
#define MIN_PITHEIGHT      3
#define MIN_PITDEPTH       6

// Block set
#define BLOCKSET_FLAT      0
#define BLOCKSET_BASIC     1
#define BLOCKSET_EXTENDED  2

// Animation speed
#define ASPEED_SLOW        0
#define ASPEED_FAST        10

// Face transparency
#define FTRANS_MIN         0 
#define FTRANS_MAX         10

// Game state
#define GAME_PLAYING       1
#define GAME_PAUSED        2
#define GAME_OVER          3
#define GAME_DEMO          4

// Screen resoltion
#define RES_640x480        0
#define RES_800x600        1
#define RES_1024x768       2
#define RES_1280x1024      3
#define RES_1600x1200      4

// Game style
#define STYLE_CLASSIC      0
#define STYLE_MARBLE       1
#define STYLE_ARCADE       2

// Game sound
#define SOUND_BLOCKOUT2    0
#define SOUND_BLOCKOUT     1

// Frame limiter
#define FR_NOLIMIT     0
#define FR_LIMIT50     1
#define FR_LIMIT60     2
#define FR_LIMIT75     3
#define FR_LIMIT100    4
#define FR_LIMITVSYNC  5

// Line width
#define LINEW_MIN    0 
#define LINEW_MAX    10

//-----------------------------------------------------------------------------
// Structure definitions
//-----------------------------------------------------------------------------
typedef struct {

  int x;
  int y;

} POINT2D;

typedef struct  {

  float x;
  float y;
  float z;

} VERTEX;

typedef struct {

  int x;
  int y;
  int z;

} BLOCKITEM;

typedef struct {

  BLOCKITEM p1;
  BLOCKITEM p2;
  int orientation;

} EDGE;

typedef struct {

  VERTEX p;
  VERTEX o;

} CORNER;

typedef struct {

  int r0;
  int r1;
  int r2;

} ORIENTATION;

typedef struct SCORERECLINK {

  int32  setupId;
  int32  score;
  int32  nbCube;
  int32  nbLine1;
  int32  nbLine2;
  int32  nbLine3;
  int32  nbLine4;
  int32  nbLine5;
  int32  startLevel;
  uint32 date;
  char   name[11];
  BYTE   emptyPit;
  int32  scoreId;
  float  gameTime;

  SCORERECLINK *next;

} SCOREREC;

typedef struct {

  char  name[11];
  int32 rank;
  int32 highScore;

} PLAYER_INFO;

typedef struct {

  int32 rotate;
  int32 tx;
  int32 ty;
  int32 tz;

} AI_MOVE;

//-----------------------------------------------------------------------------
// Util functions
//-----------------------------------------------------------------------------

extern VERTEX v(float x,float y,float z);
extern void Normalize(VERTEX *v);
extern int fround(float x);
extern char *FormatTime(float seconds);
extern char *FormatDate(uint32 time);
extern char *FormatDateShort(uint32 time);
extern int CreateTexture(int width,int height,char *imgName,GLuint *hmap);
extern char GetChar(BYTE *keys);
#ifndef WINDOWS
extern void ZeroMemory(void *buff,int size);
#endif
extern BOOL DirExists(char *dirname);
extern BOOL CheckEnv();
extern char *LID(char *fileName);
extern char *LHD(char *fileName);


#endif /* TYPESH */
