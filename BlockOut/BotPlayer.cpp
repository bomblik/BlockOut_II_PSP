/*
  File:       BotPlayer.cpp
  Description: AI player (Miscelaneaous management)
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

#include "BotPlayer.h"
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// All possible 90 deg rotations => 24 orientations
/*
const int allRot[] = { 0,0,0,0,
                       1,0,0,0,
                       1,1,0,0,
                       4,0,0,0,
                       2,0,0,0,
                       2,1,0,0,
                       2,1,1,0,
                       2,4,0,0,
                       2,2,0,0,
                       2,2,1,0,
                       2,2,1,1,
                       2,2,4,0,
                       5,0,0,0,
                       5,1,0,0,
                       5,1,1,0,
                       5,4,0,0,
                       3,0,0,0,
                       3,1,0,0,
                       3,1,1,0,
                       3,4,0,0,
                       6,0,0,0,
                       6,1,0,0,
                       6,1,1,0,
                       6,4,0,0 };
*/

// Reordered allRot: Longest transform at the end
// Simplified 2,2,1,1 to 3,3,0,0

const int allRot[] = { 0,0,0,0,
                       1,0,0,0,
                       2,0,0,0,
                       3,0,0,0,
                       4,0,0,0,
                       5,0,0,0,
                       6,0,0,0,
                       1,1,0,0,
                       2,2,0,0,
                       3,3,0,0,
                       2,1,0,0,
                       2,4,0,0,
                       3,1,0,0,
                       3,4,0,0,
                       5,1,0,0,
                       5,4,0,0,
                       6,1,0,0,
                       6,4,0,0,
                       2,1,1,0,
                       2,2,1,0,
                       2,2,4,0,
                       3,1,1,0,
                       5,1,1,0,
                       6,1,1,0 };

#ifdef AI_TEST

BOOL BotPlayer::FindCube(int x,int y,int z,Cube *c) {

  int i=0;
  BOOL found = FALSE;
  while(i<c->nbCube && !found) {
    found = (x==c->m[i].x) && 
            (y==c->m[i].y) && 
            (z==c->m[i].z);
    if(!found) i++;
  }
  return found;

}

void BotPlayer::AddTransform(int idx,int r0,int r1,int r2) {

  int i=0,j;
  int xmin,ymin,zmin,xmax,ymax,zmax;

  // Translate the polycube to the origin
  TransformCube();
  GetBounds(&xmin,&ymin,&zmin,&xmax,&ymax,&zmax);
  for(i=0;i<block->GetNbCube();i++) {
     transCube[i].x -= xmin;
     transCube[i].y -= ymin;
     transCube[i].z -= zmin;
  }

  // Search invariant transform
  i=0;
  BOOL found = FALSE;
  while(i<allTransform[idx].nbTransform && !found) {

    BOOL equal = TRUE;
    j=0;
    while( j<block->GetNbCube() && equal ) {
      equal = equal && FindCube(transCube[j].x,transCube[j].y,transCube[j].z,&(allTransform[idx].c[i]));
      if(equal) j++;
    }
    found = equal;
    if(!found) i++;

  }

  if( !found ) {

    // New transform
    for(j=0;j<block->GetNbCube();j++) {
      allTransform[idx].c[i].m[j].x = transCube[j].x;
      allTransform[idx].c[i].m[j].y = transCube[j].y;
      allTransform[idx].c[i].m[j].z = transCube[j].z;
    }
    allTransform[idx].c[i].r0 = r0;
    allTransform[idx].c[i].r1 = r1;
    allTransform[idx].c[i].r2 = r2;
    allTransform[idx].c[i].nbCube=j;
    allTransform[idx].nbTransform++;

  }

}

void BotPlayer::AnalyseSymmetry() {

  allTransform = (TransformSet *)malloc(NB_POLYCUBE*sizeof(TransformSet));
  memset(allTransform,0,NB_POLYCUBE*sizeof(TransformSet));
  xBlock=0;
  yBlock=0;
  zBlock=0;

  for(int i=0;i<NB_POLYCUBE;i++) {

   block = &(allPolyCube[i]);
   rotCenter = block->GetICenter();
   block->CopyCube(transCubeOrg,&nbCube);

   for(int ar=0;ar<24;ar++) {

     matBlock.Identity();
     // Rotate
     for(int r=0;r<4 && allRot[ar*4+r]!=0;r++)
       RotateBlock( allRot[ar*4+r] );
     AddTransform(i,allRot[ar*4+0],allRot[ar*4+1],allRot[ar*4+2]);

   }

 }

 int sumFlat=0;
 int sumBasic=0;
 int sumExtended=0;

 for(int i=0;i<NB_POLYCUBE;i++) {
   if(allPolyCube[i].IsInSet(BLOCKSET_FLAT)) sumFlat += allTransform[i].nbTransform;
   if(allPolyCube[i].IsInSet(BLOCKSET_BASIC)) sumBasic += allTransform[i].nbTransform;
   sumExtended += allTransform[i].nbTransform;
 }

 // Create the orientation array for each block
 FILE *f=fopen("BlockOrientation.h","w");
 fprintf(f,"/*\n");
 fprintf(f," File:        BlockOrientation.h\n");
 fprintf(f," Description: Distinct orientation of block\n");
 fprintf(f," Program:     BlockOut\n");
 fprintf(f," Author:      Jean-Luc PONS\n");
 fprintf(f,"\n");
 fprintf(f," This program is free software; you can redistribute it and/or modify\n");
 fprintf(f," it under the terms of the GNU General Public License as published by\n");
 fprintf(f," the Free Software Foundation; either version 2 of the License, or\n");
 fprintf(f," (at your option) any later version.\n");
 fprintf(f,"\n");
 fprintf(f," This program is distributed in the hope that it will be useful,\n");
 fprintf(f," but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
 fprintf(f," MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
 fprintf(f," GNU General Public License for more details.\n");
 fprintf(f,"*/\n\n");

 for(int i=0;i<NB_POLYCUBE;i++) {
   fprintf(f,"// Polycube No %d (%d orientation(s))\n",i,allTransform[i].nbTransform);
   for(int j=0;j<allTransform[i].nbTransform;j++)
     fprintf(f,"allPolyCube[%d].AddOrientation(%d,%d,%d);\n",i,allTransform[i].c[j].r0,allTransform[i].c[j].r1,allTransform[i].c[j].r2);
 }
 fclose(f);

 printf("Flat     => %g\n",24.0 / ((double)sumFlat/8.0) );
 printf("Basic    => %g\n",24.0 / ((double)sumBasic/7.0) );
 printf("Extended => %g\n",24.0 / ((double)sumExtended/41.0) );

 free(allTransform);

}

#endif

//-----------------------------------------------------------------------------
// inilined utils function
//-----------------------------------------------------------------------------
inline int ifround(float x) {

  int i;
  if( x<0.0f ) {
    i = (int)( (-x) + 0.5f );
    i = -i;
  } else {
    i = (int)( x + 0.5f );
  }

  return i;

}

//-----------------------------------------------------------------------------
// BotPlayer class
//-----------------------------------------------------------------------------
BotPlayer::BotPlayer() {

    matrix = NULL;
    matrixOrg = NULL;
    edgeMatrix = NULL;

#ifdef AI_TEST
    // Init polycube
    InitPolyCube();
#endif

}

void BotPlayer::Init(int w,int h,int d,int blockSet) {

  width  = w;
  height = h;
  depth  = d;
  this->blockSet = blockSet;

  if( matrix ) { 
    free( matrix );
    matrix = NULL;
  }
  if( matrixOrg ) { 
    free( matrixOrg );
    matrixOrg = NULL;
  }
  if( edgeMatrix ) {
    free(edgeMatrix);
    edgeMatrix= NULL;
  }
  mSize = width * height * depth;
  if( mSize == 0 ) {
    // Invalid dimension
    return;
  }

  matrix = (int *)malloc( mSize*sizeof(int) );
  matrixOrg = (int *)malloc( mSize*sizeof(int) );
  edgeMatrix = (float *)malloc( mSize*sizeof(float) );
  area = width*height;
  InitCoef();

}

inline void BotPlayer::SetValue(int x,int y,int z,int value) {

  if( x>=0 && x<width && y>=0 && y<height && z>=0 && z<depth ) {
    matrix[x + y*width + z*area] = value;
  }

}

int BotPlayer::GetValue(int x,int y,int z) {

  if( x<0 || x>=width || y<0 || y>=height || z<0 || z>=depth )
    return 1;
  else
    return matrix[x + y*width + z*area];

}

void BotPlayer::RestorePit() {
  memcpy(matrix,matrixOrg,mSize*sizeof(int));
}

void BotPlayer::CopyPit(Pit *pit) {

  memset(matrix,0,mSize*sizeof(int));
  for(int i=0;i<width;i++) {
    for(int j=0;j<height;j++) {
      for(int k=0;k<depth;k++) {
        if( pit->GetValue(i,j,k) ) SetValue(i,j,k,1);
      }
    }
  }
  memcpy(matrixOrg,matrix,mSize*sizeof(int));

  if(block) {
    block->CopyCube(transCubeOrg,&nbCube);
    rotCenter = block->GetICenter();
  }

}

void BotPlayer::RotateBlock(int rotation) {

  switch( rotation ) {
  case 1:
    matBlock.RotateOX();
    break;
  case 2:
    matBlock.RotateOY();
    break;
  case 3:
    matBlock.RotateOZ();
    break;
  case 4:
    matBlock.RotateNOX();
    break;
  case 5:
    matBlock.RotateNOY();
    break;
  case 6:
    matBlock.RotateNOZ();
    break;
  }

}

#define ROTATE_FULL(r)                                             \
if((r) && validMove) {                                             \
  RotateBlock( r );                                                \
  TransformCube();                                                 \
  GetBounds(&xmin,&ymin,&zmin,&xmax,&ymax,&zmax);                  \
  tx = ty = tz = 0;                                                \
  if( xmin<0 ) tx = -xmin;                                         \
  if( ymin<0 ) ty = -ymin;                                         \
  if( zmin<0 ) tz = -zmin;                                         \
  if( xmax>=width  ) tx = width  - xmax - 1;                       \
  if( ymax>=height ) ty = height - ymax - 1;                       \
  if( zmax>=depth  ) tz = depth  - zmax - 1;                       \
  if( tx || ty || tz ) {                                           \
    TranslateCube(tx,ty,tz);                                       \
    GetBounds(&xmin,&ymin,&zmin,&xmax,&ymax,&zmax);                \
  }                                                                \
  validMove = IsValidPos();                                        \
  if( validMove ) {                                                \
    curPath[nbCurPath].rotate = r;                                 \
    curPath[nbCurPath].tx = tx;                                    \
    curPath[nbCurPath].ty = ty;                                    \
    curPath[nbCurPath].tz = tz;                                    \
    nbCurPath++;                                                   \
  }                                                                \
}

#define TO_IDENTITY()                                              \
  nbCurPath = 0;                                                   \
  matBlock.Identity();                                             \
  xBlock = x;                                                      \
  yBlock = y;                                                      \
  zBlock = z;                                                      \
  TransformCube();                                                 \
  GetBounds(&xmin,&ymin,&zmin,&xmax,&ymax,&zmax);                  \
  validMove = IsValidPos();


void BotPlayer::GetMoves(Pit *pit,PolyCube *p,int x,int y,int z,AI_MOVE *moves,int *nbMove) {

  int xmin,ymin,zmin,xmax,ymax,zmax;
  int tx,ty,tz;
  BOOL validMove;

  // Initialise default
  block  = p;
  nbBestPath = 0;
  bestNote = -1e30f;
  CopyPit(pit);
  nbCubeInPit = GetNbCubeInPit() + nbCube;

  freeDepth = GetFreeDepth();

  int mDim = block->GetMaxDim();
  if( freeDepth>=mDim ) {

    for(int ar=0;ar<block->GetNbOrientation();ar++) {
      TO_IDENTITY();
      ROTATE_FULL(block->GetOrientationAt(ar)->r0);
      ROTATE_FULL(block->GetOrientationAt(ar)->r1);
      ROTATE_FULL(block->GetOrientationAt(ar)->r2);
      if( validMove ) AnalysePlane(pit,-xmin,-ymin,width-xmax-1,height-ymax-1);
    }

  } else {

    // Back to 24 orientations search
    // This prevent from few collisions at the top of the pit
    for(int ar=0;ar<24;ar++) {
      TO_IDENTITY();
      ROTATE_FULL(allRot[ar*4+0]);
      ROTATE_FULL(allRot[ar*4+1]);
      ROTATE_FULL(allRot[ar*4+2]);
      if( validMove ) AnalysePlane(pit,-xmin,-ymin,width-xmax-1,height-ymax-1);
    }

  }

  // Return best path
  *nbMove=nbBestPath;
  memcpy(moves,bestPath,nbBestPath * sizeof(AI_MOVE));

}

int BotPlayer::GetNbCubeInPit() {

  // Count number of cube in the pit
  int total=0;
  for(int i=0;i<width;i++) {
    for(int j=0;j<height;j++) {
      for(int k=0;k<depth;k++) {
        if (GET_VALUE(i,j,k)) total++;
      }
    }
  }
  return total;

}

char *BotPlayer::GetInfo(Pit *pit,BLOCKITEM *cubes,int nbCube) {

  // Initialise default
  freeDepth = GetFreeDepth();
  this->nbCube=nbCube;
  block = NULL;
  CopyPit(pit);
  nbCubeInPit = GetNbCubeInPit() + nbCube;
  memcpy(transCube,cubes,nbCube*sizeof(BLOCKITEM));
  matBlock.Identity();
  xBlock = 0;
  yBlock = 0;
  zBlock = 0;
  Evaluate();
  return infoStr;

}

inline int BotPlayer::GetDelta(int x) {
  if( x<0 ) return -1;
  if( x==0) return 0;
  return 1;
}

void BotPlayer::AnalysePlane(Pit *pit,int sX,int sY,int eX,int eY) {

  int xOrg = xBlock;
  int yOrg = yBlock;
  int zOrg = zBlock;
  int nb = nbCurPath;
  BOOL validMove;

  // Evaluate all possible translation
  for(int x=sX;x<=eX;x++) {
    for(int y=sY;y<=eY;y++) {

      nbCurPath = nb;
      xBlock = xOrg;
      yBlock = yOrg;
      zBlock = zOrg;
      TransformCube();

      // Compute translation to reach desired position
      int dX = GetDelta(x);
      int dY = GetDelta(y);
      int x1 = 0;
      int y1 = 0;
      validMove = TRUE;

      while( (x1!=x || y1!=y) && validMove ) {

        memset(curPath + nbCurPath , 0 , sizeof(AI_MOVE));       
        if( x1!=x ) { x1 += dX;  curPath[nbCurPath].tx = dX; }
        if( y1!=y ) { y1 += dY;  curPath[nbCurPath].ty = dY; }
        xBlock = xOrg + x1;
        yBlock = yOrg + y1;
        TransformCube();
        validMove = IsValidPos();
        if( validMove ) nbCurPath++;

      }
      
      if( validMove ) {

        // Evaluate
        float note = Evaluate();
        if( (note>bestNote) || (note==bestNote && nbCurPath<nbBestPath) ) {
          // Stored best path
          memcpy(bestPath,curPath,nbCurPath * sizeof(AI_MOVE));
          nbBestPath = nbCurPath;
          bestNote = note;
        }

        // Restore Pit
        RestorePit();

      }

    }
  }

}

BOOL BotPlayer::IsValidPos() {

    BOOL overlap = FALSE;

    // Check overlap between the polycube and the pit
    for(int i=0;i<nbCube && !overlap;i++) {
      if( GetValue( transCube[i].x , transCube[i].y , transCube[i].z ) )
        overlap = TRUE;
    }
    return !overlap;

}

void BotPlayer::AddBlock() {

  for(int i=0;i<nbCube;i++) 
    SetValue( transCube[i].x , transCube[i].y , transCube[i].z , 2 );

}

inline void BotPlayer::TranslateCube(int tx,int ty,int tz) {

  xBlock+=tx;
  yBlock+=ty;
  zBlock+=tz;

  for(int i=0;i<nbCube;i++) {

     transCube[i].x += tx;
     transCube[i].y += ty;
     transCube[i].z += tz;

  }

}

inline void BotPlayer::TransformCube() {

   float vx,vy,vz,rx,ry,rz;

   memcpy(transCube,transCubeOrg,nbCube*sizeof(BLOCKITEM));
   for(int i=0;i<nbCube;i++) {
   
     vx = (float)(transCube[i].x - rotCenter.x) + 0.5f;
     vy = (float)(transCube[i].y - rotCenter.y) + 0.5f;
     vz = (float)(transCube[i].z - rotCenter.z) + 0.5f;

     rx = vx * matBlock._11 + vy * matBlock._21 + vz * matBlock._31;
     ry = vx * matBlock._12 + vy * matBlock._22 + vz * matBlock._32;
     rz = vx * matBlock._13 + vy * matBlock._23 + vz * matBlock._33;

     transCube[i].x = ifround( rx - 0.5f ) + rotCenter.x + xBlock;
     transCube[i].y = ifround( ry - 0.5f ) + rotCenter.y + yBlock;
     transCube[i].z = ifround( rz - 0.5f ) + rotCenter.z + zBlock;

   }

}

inline void BotPlayer::GetBounds(int *xmin,int *ymin,int *zmin,int *xmax,int *ymax,int *zmax) {

   *xmin = 100;
   *ymin = 100;
   *zmin = 100;
   *xmax = -100;
   *ymax = -100;
   *zmax = -100;

   for(int i=0;i<nbCube;i++) {
     if( transCube[i].x > *xmax ) *xmax = transCube[i].x;
     if( transCube[i].x < *xmin ) *xmin = transCube[i].x;
     if( transCube[i].y > *ymax ) *ymax = transCube[i].y;
     if( transCube[i].y < *ymin ) *ymin = transCube[i].y;
     if( transCube[i].z > *zmax ) *zmax = transCube[i].z;
     if( transCube[i].z < *zmin ) *zmin = transCube[i].z;
   }

}

BOOL BotPlayer::IsLineFull(int z) {

  BOOL full = TRUE;
  int *lMatrix;
  
  for(int j=0;j<height && full;j++) {
    lMatrix = matrix + (z*area + j*width);
    for(int i=0;i<width && full;i++) {    
      full = full && lMatrix[i];
    }
  }
  return full;

}

void BotPlayer::RemoveLine(int idx) {

  int *lMatrix;

  for(int k=idx;k>0;k--) {
    for(int j=0;j<height;j++) {
      lMatrix = matrix + ((k-1)*area + j*width);
      for(int i=0;i<width;i++)
        SetValue(i,j,k, lMatrix[i] );
    }
  }

  // Clear last line
  memset(matrix,0,width*height*sizeof(int));

}

float BotPlayer::RemoveLines() {

  int nbRemoved = 0;
  int k=depth-1;

  while(k>=0) {
    if( IsLineFull(k) ) {
      RemoveLine(k);
      nbRemoved++;
    } else {
      k--;
    }
  }

  return (float)nbRemoved;

}

float  BotPlayer::GetNbCoveredHole() {

  int cover = 0;
  int *cMatrix;
  int start;

  for(int i=0;i<width;i++) {
    for(int j=0;j<height;j++) {
    
      cMatrix = matrix + (i + j*width);
    
      BOOL columnEmpty = TRUE;
      for(int k=0;k<depth;k++) {
        if( *cMatrix ) {
          if(columnEmpty) start = k;
          columnEmpty = FALSE;
        } else {
          if( !columnEmpty ) {
            cover+=k-start-1;
            k=depth; // Next column
          }
        }
        cMatrix += area;
      }
      
    }
  }

  return (float)cover;

}

float  BotPlayer::GetNbHole() {

  int nbHole = 0;
  int *cMatrix;

  for(int i=0;i<width;i++) {
    for(int j=0;j<height;j++) {
    
      cMatrix = matrix + (i + j*width);
    
      BOOL columnEmpty = TRUE;
      for(int k=0;k<depth;k++) {
        if( *cMatrix ) {
          columnEmpty = FALSE;
        } else {
          if( !columnEmpty ) nbHole++;
        }
        cMatrix += area;
      }
      
    }
  }

  return (float)nbHole;

}

int  BotPlayer::GetFreeDepth() {

  int fDepth = 0;

  for(int k=0;k<depth;k++) {

    BOOL lineEmpty = TRUE;
    for(int i=0;i<width && lineEmpty;i++)
     for(int j=0;j<height && lineEmpty;j++)
        lineEmpty = lineEmpty && ( GET_VALUE(i,j,k)==0 );

    if( lineEmpty ) fDepth++;

  }

  return fDepth;

}

void BotPlayer::DropBlock() {

  // Drop the block
  int dropPos = 0;
  do {
    dropPos++;
    TranslateCube(0,0,1);
  } while( IsValidPos() );
  dropPos--;
  TranslateCube(0,0,-1);

}

// ----------------------------------------------------------------------

#ifdef AI_TEST

// --------------------------------------------------
// Test function
// --------------------------------------------------
int  BotPlayer::Test(int limit,int *Seed) {

    Pit pit;

    // Init pits
    pit.SetDimension(width,height,depth);
    pit.Clear();
    nbPlayedCube = 0;

    // Get possible block
    nbPossible = 0;
    int minDim = pit.GetWidth();
    if( pit.GetHeight() < minDim ) minDim = pit.GetHeight();
    for(int i=0;i<NB_POLYCUBE;i++) {
      if( allPolyCube[i].IsInSet(blockSet) ) {
        if( allPolyCube[i].GetMaxDim() <= minDim ) {
          possible[nbPossible] = i;
          nbPossible++;
        }
      }
    }

     // Set seed
    int seed = GetTickCount()+rand()*100;
    srand(seed);

#ifdef DEPTH_STATS
    FILE *avgFile;
    int   moveCount;
    int   count[18][3];
    int   countc[18];

    avgFile = fopen("fDepth.txt","w");
    moveCount = 0;
    memset(count,0,sizeof count);
    memset(countc,0,sizeof countc);
#endif

    //Play the game

#ifdef DEPTH_STATS
    SelectPolyCube();
    BOOL gameOver = FALSE;
    while(!gameOver && nbPlayedCube<=limit && moveCount<=4000000) {


      block=NULL;CopyPit(&pit);
      int f0 = GetFreeDepth();

      gameOver = !Move(&pit);

      if( !gameOver ) {
        block=NULL;CopyPit(&pit);
        int f1 = GetFreeDepth();
        int d  = f1-f0;

        if( d > 0 ) {
          count[depth-f0][0]++;
        } else if(d==0) {
          count[depth-f0][1]++;
        } else {
          count[depth-f0][2]++;
        }
        countc[depth-f0]++;
        SelectPolyCube();
        if(((moveCount%10000)==0) && moveCount>0) {
          fprintf(avgFile,"v.add(%d,%g);\n",moveCount,(double)(count[4][2])/(double)(countc[4]));
          srand(GetTickCount()+rand()*100);
        }
        moveCount++;
      }

    }

    fprintf(avgFile,"block: %d\n\n",moveCount);
    fprintf(avgFile,"        -D       0      +D     Hits\n",moveCount-1);
    for(int i=0;i<18;i++) {
      fprintf(avgFile,"%2d => %7.5lf %7.5lf %7.5lf (%d)\n",i,(double)(count[i][0])/(double)(countc[i]),
                                            (double)(count[i][1])/(double)(countc[i]),
                                            (double)(count[i][2])/(double)(countc[i]),countc[i]);
    }
    fclose(avgFile);
#else
    SelectPolyCube();
    while(Move(&pit) && nbPlayedCube<=limit) SelectPolyCube();
#endif

    if(Seed)
      *Seed = seed;

    return nbPlayedCube;

}

int BotPlayer::Move(Pit *pit) {

  AI_MOVE  AIMoves[MAX_MOVE];
  int      nbAIMove;
  
  // Translate the polycube to the left
  int xPos = pit->GetWidth() - allPolyCube[pIdx].GetWidth();

  // Get move
  GetMoves(pit,&(allPolyCube[pIdx]),xPos,0,0,AIMoves,&nbAIMove);

  // Transform
  matBlock.Identity();
  xBlock = xPos;
  yBlock = 0;
  zBlock = 0;
  for(int i=0;i<nbAIMove;i++) {
    RotateBlock(AIMoves[i].rotate);
    xBlock += AIMoves[i].tx;
    yBlock += AIMoves[i].ty;
    zBlock += AIMoves[i].tz;
  }
  TransformCube();

  if( !IsValidPos() ) {
    // Game over
    return 0;
  }

  //Drop
  DropBlock();

  // Add the polycube to the pit
  for(int i=0;i<nbCube;i++)
    pit->AddCube( transCube[i].x , transCube[i].y , transCube[i].z );

  // Remove lines
  pit->RemoveLines();
  nbPlayedCube += allPolyCube[pIdx].GetNbCube();

  return 1;

}


void BotPlayer::SelectPolyCube() {

// Get a new polycube
  pIdx = possible[rand() % nbPossible];

}

#endif
