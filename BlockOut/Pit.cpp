/*
  File:        Pit.cpp
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

#include "Pit.h"

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }


//-----------------------------------------------------------------------------

Pit::Pit() {

  gridList = 0;
  backList = 0;
  sideList = 0;
  cubeList = 0;
  hGridList = 0;
  for(int i=0;i<12;i++)
    lcubeList[i] = 0;
  for(int i=0;i<NBTCUBE;i++) 
    cubeTList[i]= 0;

  width = 0;
  height = 0;
  depth = 0;
  mSize = 0;
  style = 0;
  cubeSide = 0.0f;
  matrix = NULL;
  orderMatrix = NULL;
  inited = FALSE;
  backTexture = 0;
  cubeTexture = 0;
  pitLevelSurf = NULL;

}

//-----------------------------------------------------------------------------

void Pit::SetDimension(int gWidth,int gHeight,int gDepth) {

  width  = gWidth;
  height = gHeight;
  depth  = gDepth;

  // Init pit matrix
  if( matrix ) {
    free(matrix);
    matrix = NULL;
  }
  if( orderMatrix ) {
    free(orderMatrix);
    orderMatrix = NULL;
  }

  mSize = width * height * depth;
  if( mSize == 0 ) {
    // Invalid dimension
    return;
  }
  matrix = (int *)malloc( mSize*sizeof(int) );
  Clear();
  InitOrderMatrix();

}

//-----------------------------------------------------------------------------

BOOL Pit::IsEmpty() {

  int i = 0;
  BOOL empty = TRUE;
  while(empty && i<depth) {
    empty = IsLineEmpty(i);
    i++;
  }
  return empty;

}

//-----------------------------------------------------------------------------

int Pit::RemoveLines() {

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

  return nbRemoved;

}

//-----------------------------------------------------------------------------

void Pit::RemoveLine(int idx) {

  for(int k=idx;k>0;k--) {
    for(int i=0;i<width;i++)
      for(int j=0;j<height;j++)
        SetValue(i,j,k, GetValue(i,j,k-1) );
  }

  // Clear last line
  for(int i=0;i<width;i++)
    for(int j=0;j<height;j++)
      SetValue(i,j,0,0);

}

//-----------------------------------------------------------------------------

int Pit::Create(int scrWidth,int scrHeight,int style) {

  float startX;
  float startY;

  // Check dimension
  if( width*height*depth == 0 ) {
    // Invalid dimension
    // Does not create but return OK
    return GL_OK;
  }

  this->style = style;

  // Init global variable

  if( width>height ) {
    cubeSide = 1.0f / (float)width;
    startX = -0.5f;
    startY = -0.5f + ( (float)(width-height)*cubeSide/2.0f );
  } else {
    cubeSide = 1.0f / (float)height;
    startX = -0.5f + ( (float)(height-width)*cubeSide/2.0f );
    startY = -0.5f;
  }

  fWidth  = (float)width  * cubeSide;
  fHeight = (float)height * cubeSide;
  fDepth  = (float)depth  * cubeSide;

  origin.x = startX;
  origin.y = startY;
  origin.z = STARTZ;

  // Init material
  memset (&blackMaterial, 0, sizeof (GLMATERIAL));

  memset (&gridMaterial, 0, sizeof (GLMATERIAL));
  gridMaterial.Diffuse.r = 1.0f;
  gridMaterial.Diffuse.g = 1.0f;
  gridMaterial.Diffuse.b = 1.0f;
  gridMaterial.Ambient.r = 0.0f;
  gridMaterial.Ambient.g = 0.6f;
  gridMaterial.Ambient.b = 0.0f;
  memset (&darkMaterial, 0, sizeof (GLMATERIAL));
  darkMaterial.Diffuse.r = 0.1f;
  darkMaterial.Diffuse.g = 0.1f;
  darkMaterial.Diffuse.b = 0.1f;
  darkMaterial.Ambient.r = 0.1f;
  darkMaterial.Ambient.g = 0.1f;
  darkMaterial.Ambient.b = 0.1f;
  memset (&backMaterial, 0, sizeof (GLMATERIAL));
  backMaterial.Diffuse.r = 0.05f;
  backMaterial.Diffuse.g = 0.05f;
  backMaterial.Diffuse.b = 0.1f;
  backMaterial.Ambient.r = 0.05f;
  backMaterial.Ambient.g = 0.05f;
  backMaterial.Ambient.b = 0.1f;
  memset (&backTexMaterial, 0, sizeof (GLMATERIAL));
  backTexMaterial.Diffuse.r = 0.35f;
  backTexMaterial.Diffuse.g = 0.35f;
  backTexMaterial.Diffuse.b = 0.35f;
  backTexMaterial.Ambient.r = 0.35f;
  backTexMaterial.Ambient.g = 0.35f;
  backTexMaterial.Ambient.b = 0.35f;
  memset (&whiteMaterial, 0, sizeof (GLMATERIAL));
  whiteMaterial.Diffuse.r = 1.0f;
  whiteMaterial.Diffuse.g = 1.0f;
  whiteMaterial.Diffuse.b = 1.0f;
  whiteMaterial.Ambient.r = 1.0f;
  whiteMaterial.Ambient.g = 1.0f;
  whiteMaterial.Ambient.b = 1.0f;

  // Init 3D objects
  if( !CreateGrid() ) return GL_FAIL;
  if( !CreateBack() ) return GL_FAIL;
  if( !CreateFillingCube() ) return GL_FAIL;
  if( !CreatePitLevel(scrWidth,scrHeight) ) return GL_FAIL;

  inited = TRUE;
  return GL_OK;

}

//-----------------------------------------------------------------------------

void Pit::InitOrderMatrix() {

  double *dist;
  double xOrg = (double)width/2.0;
  double yOrg = (double)height/2.0;
  double zOrg = 0.0;

  orderMatrix = (BLOCKITEM *)malloc( sizeof(BLOCKITEM)*mSize );
  dist = (double *)malloc( sizeof(double)*mSize );

  // Init distance
  int l = 0;
  for(int k=0;k<depth;k++) {
    for(int j=0;j<height;j++) {
      for(int i=0;i<width;i++) {
        double xc = double(2*i+1)/2.0;
        double yc = double(2*j+1)/2.0;
        double zc = double(2*k+1)/2.0;
        dist[l] = (xc-xOrg)*(xc-xOrg) + 
                  (yc-yOrg)*(yc-yOrg) +
                  (zc-zOrg)*(zc-zOrg);
        orderMatrix[l].x = i;
        orderMatrix[l].y = j;
        orderMatrix[l].z = k;
        l++;
      }
    }
  }

  // Sort
  BOOL end = FALSE;
  int i=0;
  int j=mSize-1;

  while(!end) {
    end = TRUE;
    for(i=0;i<j;i++) {
      if( dist[i]<dist[i+1] ) {

        // Swap dist
        double tmp = dist[i];
        dist[i] = dist[i+1];
        dist[i+1] = tmp;
        // Swap orderMatrix
        BLOCKITEM tmp2 = orderMatrix[i];
        orderMatrix[i] = orderMatrix[i+1];
        orderMatrix[i+1] = tmp2;

        end = FALSE;
      }
    }
    j--;
  }

  free(dist);

}

//-----------------------------------------------------------------------------

int Pit::CreateBack() {

  switch(style) {

  // ----- CLASSIC Style --------------------------------------------------------------
  case STYLE_CLASSIC:

  backList = glGenLists(1);
  glNewList(backList,GL_COMPILE);
  glBegin(GL_QUADS);

#ifndef PLATFORM_PSP
  // Face 1
  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f(origin.x , origin.y + fHeight , origin.z);
  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f(origin.x , origin.y + fHeight , origin.z + fDepth);
  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f(origin.x , origin.y           , origin.z + fDepth);
  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f(origin.x , origin.y           , origin.z );
#endif

  // Face 2
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f(origin.x          , origin.y + fHeight , origin.z);
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f(origin.x + fWidth , origin.y + fHeight , origin.z);
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f(origin.x + fWidth , origin.y + fHeight , origin.z + fDepth);
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f(origin.x          , origin.y + fHeight , origin.z + fDepth);

  // Face 3
  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f(origin.x + fWidth , origin.y + fHeight , origin.z + fDepth);
  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f(origin.x + fWidth , origin.y + fHeight , origin.z);
  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f(origin.x + fWidth , origin.y           , origin.z);
  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f(origin.x + fWidth , origin.y           , origin.z + fDepth);

  // Face 4
  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f(origin.x + fWidth , origin.y , origin.z);
  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f(origin.x          , origin.y , origin.z);
  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f(origin.x          , origin.y , origin.z + fDepth);
  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f(origin.x + fWidth , origin.y , origin.z + fDepth);

  // Face 5
  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f(origin.x          , origin.y + fHeight , origin.z + fDepth);
  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f(origin.x + fWidth , origin.y + fHeight , origin.z + fDepth);
  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f(origin.x + fWidth , origin.y           , origin.z + fDepth);
  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f(origin.x          , origin.y           , origin.z + fDepth);

  glEnd();
  glEndList();

  break;

  // ----- MARBLE Style --------------------------------------------------------------
  case STYLE_MARBLE: {

  float mx = (float)width /4.0f;
  float my = (float)height/4.0f;
  float mz = (float)depth /4.0f;

  backList = glGenLists(1);
  glNewList(backList,GL_COMPILE);
  glBegin(GL_QUADS);

  // Face 1
  glTexCoord2f(mz   , 0.0f);
  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f(origin.x , origin.y + fHeight , origin.z);
  glTexCoord2f(0.0f , 0.0f);
  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f(origin.x , origin.y + fHeight , origin.z + fDepth);
  glTexCoord2f(0.0f , my);
  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f(origin.x , origin.y           , origin.z + fDepth);
  glTexCoord2f(mz   , my);
  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f(origin.x , origin.y           , origin.z);

  // Face 2
  glTexCoord2f(mz   , mx);
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f(origin.x          , origin.y + fHeight , origin.z);
  glTexCoord2f(mz   , 0.0f);
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f(origin.x + fWidth , origin.y + fHeight , origin.z);
  glTexCoord2f(0.0f , 0.0f);
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f(origin.x + fWidth , origin.y + fHeight , origin.z + fDepth);
  glTexCoord2f(0.0f , mx);
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f(origin.x          , origin.y + fHeight , origin.z + fDepth);


  // Face 3
  glTexCoord2f(0.0f , my);
  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f(origin.x + fWidth , origin.y + fHeight , origin.z + fDepth);
  glTexCoord2f(mz   , my);
  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f(origin.x + fWidth , origin.y + fHeight , origin.z);
  glTexCoord2f(mz   , 0.0f);
  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f(origin.x + fWidth , origin.y           , origin.z);
  glTexCoord2f(0.0f , 0.0f);
  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f(origin.x + fWidth , origin.y           , origin.z + fDepth);

  // Face 4
  glTexCoord2f(mz   , mx);
  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f(origin.x + fWidth , origin.y , origin.z);
  glTexCoord2f(mz   , 0.0f);
  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f(origin.x          , origin.y , origin.z);
  glTexCoord2f(0.0f , 0.0f);
  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f(origin.x          , origin.y , origin.z + fDepth);
  glTexCoord2f(0.0f , mx);
  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f(origin.x + fWidth , origin.y , origin.z + fDepth);


  // Face 5
  glTexCoord2f(mx   , my);
  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f(origin.x          , origin.y + fHeight , origin.z + fDepth);
  glTexCoord2f(0.0f , my);
  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f(origin.x + fWidth , origin.y + fHeight , origin.z + fDepth);
  glTexCoord2f(0.0f , 0.0f);
  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f(origin.x + fWidth , origin.y           , origin.z + fDepth);
  glTexCoord2f(mx   , 0.0f);
  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f(origin.x          , origin.y           , origin.z + fDepth);

  glEnd();
  glEndList();

#ifndef PLATFORM_PSP
  // Marble texture
  if( !CreateTexture(512,512,STR("images/marbleg.png"),&backTexture) )
    return GL_FAIL;

  // Marble block texture
  if( !CreateTexture(256,256,STR("images/marble.png"),&cubeTexture) )
    return GL_FAIL;
#else
  // Marble texture
  if( !CreateTexture(512,512,STR("images.psp/marbleg.png"),&backTexture) )
    return GL_FAIL;

  // Marble block texture
  if( !CreateTexture(256,256,STR("images.psp/marble.png"),&cubeTexture) )
    return GL_FAIL;
#endif
  }
  break;

  // ----- ARCADE Style --------------------------------------------------------------
  case STYLE_ARCADE: {

  // Crystal block texture
#ifndef PLATFORM_PSP
  if( !CreateTexture(256,256,STR("images/star_crystal_grid.png"),&cubeTexture) )
#else
  if( !CreateTexture(256,256,STR("images.psp/star_crystal_grid.png"),&cubeTexture) )
#endif
   return GL_FAIL;
  }

  break;

  } // End switch(style)

  // ------------------------------------------------------------------------
  // Side (Black polygon around the pit, for faster repaint)
  // ------------------------------------------------------------------------
  sideList = glGenLists(1);
  glNewList(sideList,GL_COMPILE);
  glBegin(GL_TRIANGLES);

  glVertex3f(origin.x + fWidth , origin.y + fHeight , origin.z);                        // 2
  glVertex3f(origin.x          - cubeSide , origin.y + fHeight + cubeSide , origin.z);  // 7
  glVertex3f(origin.x + fWidth + cubeSide , origin.y + fHeight + cubeSide , origin.z);  // 6

  glVertex3f(origin.x + fWidth , origin.y + fHeight , origin.z);                        // 2
  glVertex3f(origin.x          , origin.y + fHeight , origin.z);                        // 3
  glVertex3f(origin.x          - cubeSide , origin.y + fHeight + cubeSide , origin.z);  // 7

  glVertex3f(origin.x          , origin.y + fHeight , origin.z);                        // 3
  glVertex3f(origin.x          - cubeSide , origin.y           - cubeSide , origin.z);  // 4
  glVertex3f(origin.x          - cubeSide , origin.y + fHeight + cubeSide , origin.z);  // 7

  glVertex3f(origin.x          , origin.y + fHeight , origin.z);                        // 3
  glVertex3f(origin.x          , origin.y           , origin.z);                        // 0
  glVertex3f(origin.x          - cubeSide , origin.y           - cubeSide , origin.z);  // 4

  glVertex3f(origin.x          , origin.y           , origin.z);                        // 0
  glVertex3f(origin.x + fWidth + cubeSide , origin.y           - cubeSide , origin.z);  // 5
  glVertex3f(origin.x          - cubeSide , origin.y           - cubeSide , origin.z);  // 4

  glVertex3f(origin.x          , origin.y           , origin.z);                        // 0
  glVertex3f(origin.x + fWidth , origin.y           , origin.z);                        // 1
  glVertex3f(origin.x + fWidth + cubeSide , origin.y           - cubeSide , origin.z);  // 5

  glVertex3f(origin.x + fWidth + cubeSide , origin.y           - cubeSide , origin.z);  // 5
  glVertex3f(origin.x + fWidth , origin.y           , origin.z);                        // 1
  glVertex3f(origin.x + fWidth , origin.y + fHeight , origin.z);                        // 2

  glVertex3f(origin.x + fWidth + cubeSide , origin.y           - cubeSide , origin.z);  // 5
  glVertex3f(origin.x + fWidth , origin.y + fHeight , origin.z);                        // 2
  glVertex3f(origin.x + fWidth + cubeSide , origin.y + fHeight + cubeSide , origin.z);  // 6

  glEnd();
  glEndList();

  return GL_OK;

}

//-----------------------------------------------------------------------------

int Pit::CreateGrid() {

  gridList = glGenLists(1);
  glNewList(gridList,GL_COMPILE);
  glBegin(GL_LINES);

  for(int i=0;i<=width;i++) {
  glVertex3f(origin.x + (i*cubeSide), -origin.y , origin.z);
  glVertex3f(origin.x + (i*cubeSide), -origin.y , origin.z + fDepth);
  glVertex3f(origin.x + (i*cubeSide),  origin.y , origin.z);
  glVertex3f(origin.x + (i*cubeSide),  origin.y , origin.z + fDepth);
  }

  for(int i=1;i<height;i++) {
    glVertex3f(origin.x , origin.y + (i*cubeSide), origin.z);
    glVertex3f(origin.x , origin.y + (i*cubeSide), origin.z + fDepth);
    glVertex3f(-origin.x , origin.y + (i*cubeSide), origin.z);
    glVertex3f(-origin.x , origin.y + (i*cubeSide), origin.z + fDepth);
  }

  for(int i=0;i<=depth;i++) {
    glVertex3f( origin.x , -origin.y , origin.z + (i*cubeSide));
    glVertex3f(-origin.x , -origin.y , origin.z + (i*cubeSide));
    glVertex3f(-origin.x , -origin.y , origin.z + (i*cubeSide));
    glVertex3f(-origin.x ,  origin.y , origin.z + (i*cubeSide));
    glVertex3f(-origin.x ,  origin.y , origin.z + (i*cubeSide));
    glVertex3f( origin.x ,  origin.y , origin.z + (i*cubeSide));
    glVertex3f( origin.x ,  origin.y , origin.z + (i*cubeSide));
    glVertex3f( origin.x , -origin.y , origin.z + (i*cubeSide));
  }

  for(int i=1;i<width;i++) {
    glVertex3f(origin.x + (i*cubeSide), -origin.y , origin.z + fDepth);
    glVertex3f(origin.x + (i*cubeSide),  origin.y , origin.z + fDepth);
  }

  for(int i=1;i<height;i++) {
    glVertex3f(origin.x , origin.y + (i*cubeSide), origin.z + fDepth);
    glVertex3f(-origin.x , origin.y + (i*cubeSide), origin.z + fDepth);
  }

  glEnd();
  glEndList();

  if( style == STYLE_MARBLE ) {

    hGridList = glGenLists(1);
    glNewList(hGridList,GL_COMPILE);
    glBegin(GL_LINES);

    for(int i=0;i<=depth;i++) {
      glVertex3f( origin.x , -origin.y , origin.z + (i*cubeSide));
      glVertex3f(-origin.x , -origin.y , origin.z + (i*cubeSide));
      glVertex3f(-origin.x , -origin.y , origin.z + (i*cubeSide));
      glVertex3f(-origin.x ,  origin.y , origin.z + (i*cubeSide));
      glVertex3f(-origin.x ,  origin.y , origin.z + (i*cubeSide));
      glVertex3f( origin.x ,  origin.y , origin.z + (i*cubeSide));
      glVertex3f( origin.x ,  origin.y , origin.z + (i*cubeSide));
      glVertex3f( origin.x , -origin.y , origin.z + (i*cubeSide));
    }

    glEnd();
    glEndList();

  }

  return GL_OK;

}

//-----------------------------------------------------------------------------

int Pit::CreatePitLevel(int scrWidth,int scrHeight) {

  // Get back buffer format

  pitLevelX = fround( (float)scrWidth  * 0.0176f );
  pitLevelY = fround( (float)scrHeight * 0.1510f );
  pitLevelW = fround( (float)scrWidth  * 0.0498f );
  pitLevelH = fround( (float)scrHeight * 0.8255f );
  pitLevelSurf = (GLfloat *)malloc( pitLevelW * pitLevelH * 4 * sizeof(GLfloat) );

  return GL_OK;

}

//-----------------------------------------------------------------------------

void Pit::ChooseMapping(int i,float *sx,float *sy,float *ex,float *ey) {

  float c[] = { 0.0f , 0.25f , 0.5f , 0.75f };
  float _1px = 1.0f / 256.0f;

  *sx = c[(i/4)%4];
  *sy = c[i%4];

  *ex = *sx + 0.25f;
  *ey = *sy + 0.25f;

  *sx += _1px;
  *sy += _1px;
  *ex -= _1px;
  *ey -= _1px;

}

//-----------------------------------------------------------------------------
   
GLuint Pit::CreateTexturedFillingCube(int i) {

  float sx;
  float sy;
  float ex;
  float ey;

  GLuint l = glGenLists(1);
  glNewList(l,GL_COMPILE);
  glBegin(GL_QUADS);

  // Face 1
  ChooseMapping(i,&sx,&sy,&ex,&ey);
  glTexCoord2f(sx, sy);
  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f(0 * cubeSide + origin.x,1 * cubeSide + origin.y,0 * cubeSide + origin.z);
  glTexCoord2f(ex, sy);
  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f(1 * cubeSide + origin.x,1 * cubeSide + origin.y,0 * cubeSide + origin.z);
  glTexCoord2f(ex, ey);
  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f(1 * cubeSide + origin.x,0 * cubeSide + origin.y,0 * cubeSide + origin.z);
  glTexCoord2f(sx, ey);
  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f(0 * cubeSide + origin.x,0 * cubeSide + origin.y,0 * cubeSide + origin.z);

  // Face 2
  ChooseMapping(i+NBTCUBE,&sx,&sy,&ex,&ey);
  glTexCoord2f(sx, sy);
  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f(1 * cubeSide + origin.x,1 * cubeSide + origin.y,0 * cubeSide + origin.z);
  glTexCoord2f(ex, sy);
  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f(1 * cubeSide + origin.x,1 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glTexCoord2f(ex, ey);
  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f(1 * cubeSide + origin.x,0 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glTexCoord2f(sx, ey);
  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f(1 * cubeSide + origin.x,0 * cubeSide + origin.y,0 * cubeSide + origin.z);

  // Face 3
  ChooseMapping(i+2*NBTCUBE,&sx,&sy,&ex,&ey);
  glTexCoord2f(sx, sy);
  glNormal3f(0.0f,0.0f,1.0f);
  glVertex3f(1 * cubeSide + origin.x,1 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glTexCoord2f(ex, sy);
  glNormal3f(0.0f,0.0f,1.0f);
  glVertex3f(0 * cubeSide + origin.x,1 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glTexCoord2f(ex, ey);
  glNormal3f(0.0f,0.0f,1.0f);
  glVertex3f(0 * cubeSide + origin.x,0 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glTexCoord2f(sx, ey);
  glNormal3f(0.0f,0.0f,1.0f);
  glVertex3f(1 * cubeSide + origin.x,0 * cubeSide + origin.y,1 * cubeSide + origin.z);


  // Face 4
  ChooseMapping(i+3*NBTCUBE,&sx,&sy,&ex,&ey);
  glTexCoord2f(sx, sy);
  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f(0 * cubeSide + origin.x,1 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glTexCoord2f(ex, sy);
  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f(0 * cubeSide + origin.x,1 * cubeSide + origin.y,0 * cubeSide + origin.z);
  glTexCoord2f(ex, ey);
  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f(0 * cubeSide + origin.x,0 * cubeSide + origin.y,0 * cubeSide + origin.z);
  glTexCoord2f(sx, ey);
  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f(0 * cubeSide + origin.x,0 * cubeSide + origin.y,1 * cubeSide + origin.z);

  // Face 5
  ChooseMapping(i+4*NBTCUBE,&sx,&sy,&ex,&ey);
  glTexCoord2f(sx, sy);
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f(1 * cubeSide + origin.x,0 * cubeSide + origin.y,0 * cubeSide + origin.z);
  glTexCoord2f(ex, sy);
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f(1 * cubeSide + origin.x,0 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glTexCoord2f(ex, ey);
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f(0 * cubeSide + origin.x,0 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glTexCoord2f(sx, ey);
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f(0 * cubeSide + origin.x,0 * cubeSide + origin.y,0 * cubeSide + origin.z);


  // Face 6
  ChooseMapping(i+5*NBTCUBE,&sx,&sy,&ex,&ey);
  glTexCoord2f(sx, sy);
  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f(0 * cubeSide + origin.x,1 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glTexCoord2f(ex, sy);
  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f(1 * cubeSide + origin.x,1 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glTexCoord2f(ex, ey);
  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f(1 * cubeSide + origin.x,1 * cubeSide + origin.y,0 * cubeSide + origin.z);
  glTexCoord2f(sx, ey);
  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f(0 * cubeSide + origin.x,1 * cubeSide + origin.y,0 * cubeSide + origin.z);

  glEnd();
  glEndList();

  return l;

}

//-----------------------------------------------------------------------------

int Pit::CreateFillingCube() {

  switch(style) {

  // ----- CLASSIC Style --------------------------------------------------------------
  case STYLE_CLASSIC:

  cubeList = glGenLists(1);
  glNewList(cubeList,GL_COMPILE);
  glBegin(GL_QUADS);

  // Face 1
  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f(0 * cubeSide + origin.x,1 * cubeSide + origin.y,0 * cubeSide + origin.z);
  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f(1 * cubeSide + origin.x,1 * cubeSide + origin.y,0 * cubeSide + origin.z);
  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f(1 * cubeSide + origin.x,0 * cubeSide + origin.y,0 * cubeSide + origin.z);
  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f(0 * cubeSide + origin.x,0 * cubeSide + origin.y,0 * cubeSide + origin.z);

  // Face 2
  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f(1 * cubeSide + origin.x,1 * cubeSide + origin.y,0 * cubeSide + origin.z);
  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f(1 * cubeSide + origin.x,1 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f(1 * cubeSide + origin.x,0 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f(1 * cubeSide + origin.x,0 * cubeSide + origin.y,0 * cubeSide + origin.z);

  // Face 3
  glNormal3f(0.0f,0.0f,1.0f);
  glVertex3f(1 * cubeSide + origin.x,1 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glNormal3f(0.0f,0.0f,1.0f);
  glVertex3f(0 * cubeSide + origin.x,1 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glNormal3f(0.0f,0.0f,1.0f);
  glVertex3f(0 * cubeSide + origin.x,0 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glNormal3f(0.0f,0.0f,1.0f);
  glVertex3f(1 * cubeSide + origin.x,0 * cubeSide + origin.y,1 * cubeSide + origin.z);


  // Face 4
  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f(0 * cubeSide + origin.x,1 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f(0 * cubeSide + origin.x,1 * cubeSide + origin.y,0 * cubeSide + origin.z);
  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f(0 * cubeSide + origin.x,0 * cubeSide + origin.y,0 * cubeSide + origin.z);
  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f(0 * cubeSide + origin.x,0 * cubeSide + origin.y,1 * cubeSide + origin.z);

  // Face 5
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f(1 * cubeSide + origin.x,0 * cubeSide + origin.y,0 * cubeSide + origin.z);
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f(1 * cubeSide + origin.x,0 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f(0 * cubeSide + origin.x,0 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f(0 * cubeSide + origin.x,0 * cubeSide + origin.y,0 * cubeSide + origin.z);


  // Face 6
  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f(0 * cubeSide + origin.x,1 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f(1 * cubeSide + origin.x,1 * cubeSide + origin.y,1 * cubeSide + origin.z);
  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f(1 * cubeSide + origin.x,1 * cubeSide + origin.y,0 * cubeSide + origin.z);
  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f(0 * cubeSide + origin.x,1 * cubeSide + origin.y,0 * cubeSide + origin.z);

  glEnd();
  glEndList();

  break;

  // ----- MARBLE/ARCADE Style --------------------------------------------------------------
  case STYLE_MARBLE:
  case STYLE_ARCADE:

  for(int i=0;i<NBTCUBE;i++) {
    cubeTList[i] = CreateTexturedFillingCube(i);
  }
  break;

  } // End switch(style)

  // --------------------------------------------------------------------------

  // Cube contours

  lcubeList[0] = glGenLists(1);
  glNewList(lcubeList[0],GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f(0 * cubeSide + origin.x,1 * cubeSide + origin.y,0 * cubeSide + origin.z); // 0
  glVertex3f(1 * cubeSide + origin.x,1 * cubeSide + origin.y,0 * cubeSide + origin.z); // 1
  glEnd();
  glEndList();

  lcubeList[1] = glGenLists(1);
  glNewList(lcubeList[1],GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f(1 * cubeSide + origin.x,1 * cubeSide + origin.y,0 * cubeSide + origin.z); // 1
  glVertex3f(1 * cubeSide + origin.x,0 * cubeSide + origin.y,0 * cubeSide + origin.z); // 2
  glEnd();
  glEndList();

  lcubeList[2] = glGenLists(1);
  glNewList(lcubeList[2],GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f(1 * cubeSide + origin.x,0 * cubeSide + origin.y,0 * cubeSide + origin.z); // 2
  glVertex3f(0 * cubeSide + origin.x,0 * cubeSide + origin.y,0 * cubeSide + origin.z); // 3
  glEnd();
  glEndList();

  lcubeList[3] = glGenLists(1);
  glNewList(lcubeList[3],GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f(0 * cubeSide + origin.x,0 * cubeSide + origin.y,0 * cubeSide + origin.z); // 3
  glVertex3f(0 * cubeSide + origin.x,1 * cubeSide + origin.y,0 * cubeSide + origin.z); // 0
  glEnd();
  glEndList();

  lcubeList[4] = glGenLists(1);
  glNewList(lcubeList[4],GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f(0 * cubeSide + origin.x,1 * cubeSide + origin.y,0 * cubeSide + origin.z); // 0
  glVertex3f(0 * cubeSide + origin.x,1 * cubeSide + origin.y,1 * cubeSide + origin.z); // 4
  glEnd();
  glEndList();

  lcubeList[5] = glGenLists(1);
  glNewList(lcubeList[5],GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f(1 * cubeSide + origin.x,1 * cubeSide + origin.y,0 * cubeSide + origin.z); // 1
  glVertex3f(1 * cubeSide + origin.x,1 * cubeSide + origin.y,1 * cubeSide + origin.z); // 5
  glEnd();
  glEndList();

  lcubeList[6] = glGenLists(1);
  glNewList(lcubeList[6],GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f(1 * cubeSide + origin.x,0 * cubeSide + origin.y,0 * cubeSide + origin.z); // 2
  glVertex3f(1 * cubeSide + origin.x,0 * cubeSide + origin.y,1 * cubeSide + origin.z); // 6
  glEnd();
  glEndList();

  lcubeList[7] = glGenLists(1);
  glNewList(lcubeList[7],GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f(0 * cubeSide + origin.x,0 * cubeSide + origin.y,0 * cubeSide + origin.z); // 3
  glVertex3f(0 * cubeSide + origin.x,0 * cubeSide + origin.y,1 * cubeSide + origin.z); // 7
  glEnd();
  glEndList();

  lcubeList[8] = glGenLists(1);
  glNewList(lcubeList[8],GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f(0 * cubeSide + origin.x,1 * cubeSide + origin.y,1 * cubeSide + origin.z); // 4
  glVertex3f(1 * cubeSide + origin.x,1 * cubeSide + origin.y,1 * cubeSide + origin.z); // 5
  glEnd();
  glEndList();

  lcubeList[9] = glGenLists(1);
  glNewList(lcubeList[9],GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f(1 * cubeSide + origin.x,1 * cubeSide + origin.y,1 * cubeSide + origin.z); // 5
  glVertex3f(1 * cubeSide + origin.x,0 * cubeSide + origin.y,1 * cubeSide + origin.z); // 6
  glEnd();
  glEndList();

  lcubeList[10] = glGenLists(1);
  glNewList(lcubeList[10],GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f(0 * cubeSide + origin.x,0 * cubeSide + origin.y,1 * cubeSide + origin.z); // 7
  glVertex3f(1 * cubeSide + origin.x,0 * cubeSide + origin.y,1 * cubeSide + origin.z); // 6
  glEnd();
  glEndList();

  lcubeList[11] = glGenLists(1);
  glNewList(lcubeList[11],GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f(0 * cubeSide + origin.x,0 * cubeSide + origin.y,1 * cubeSide + origin.z); // 7
  glVertex3f(0 * cubeSide + origin.x,1 * cubeSide + origin.y,1 * cubeSide + origin.z); // 4
  glEnd();
  glEndList();

  return GL_OK;

}

//-----------------------------------------------------------------------------

GLMATERIAL *Pit::GetMaterial(int level) {

  static GLMATERIAL ret;
  memset (&ret, 0, sizeof (GLMATERIAL));

  switch((depth-level-1) % 7) {

    case 0:
      ret.Diffuse.r = 0.0f;
      ret.Diffuse.g = 0.0f;
      ret.Diffuse.b = 1.0f;
      break;

    case 1:
      ret.Diffuse.r = 0.0f;
      ret.Diffuse.g = 1.0f;
      ret.Diffuse.b = 0.0f;
      break;

    case 2:
      ret.Diffuse.r = 0.0f;
      ret.Diffuse.g = 0.9f;
      ret.Diffuse.b = 0.9f;
      break;

    case 3:
      ret.Diffuse.r = 1.0f;
      ret.Diffuse.g = 0.0f;
      ret.Diffuse.b = 0.0f;
      break;

    case 4:
      ret.Diffuse.r = 1.0f;
      ret.Diffuse.g = 0.1f;
      ret.Diffuse.b = 0.8f;
      break;

    case 5:
      ret.Diffuse.r = 0.9f;
      ret.Diffuse.g = 0.6f;
      ret.Diffuse.b = 0.0f;
      break;

    case 6:
      ret.Diffuse.r = 0.85f;
      ret.Diffuse.g = 0.85f;
      ret.Diffuse.b = 0.85f;
      break;

  }

  ret.Ambient = ret.Diffuse;

  ret.Diffuse.r *= 0.3f;
  ret.Diffuse.g *= 0.3f;
  ret.Diffuse.b *= 0.3f;

  ret.Ambient.r *= 0.7f;
  ret.Ambient.g *= 0.7f;
  ret.Ambient.b *= 0.7f;

  return &ret;

}
//-----------------------------------------------------------------------------

BOOL Pit::IsVisible(int x,int y,int z) {
  
  if( z==0 ) return TRUE;

  return ( GetValue(x+1,y,z)==0 ||
           GetValue(x-1,y,z)==0 ||
           GetValue(x,y+1,z)==0 ||
           GetValue(x,y-1,z)==0 ||
           GetValue(x,y,z-1)==0 );

}

//-----------------------------------------------------------------------------

BOOL Pit::IsVisible2(int x,int y,int z) {
  
  if( z==0 ) return TRUE;

  return ( GetValue2(x+1,y,z)==0 ||
           GetValue2(x-1,y,z)==0 ||
           GetValue2(x,y+1,z)==0 ||
           GetValue2(x,y-1,z)==0 ||
           GetValue2(x,y,z-1)==0 );

}

//-----------------------------------------------------------------------------

void Pit::Clear() {

  if( matrix )
    memset( matrix , 0 , mSize*sizeof(int) );
  tcubeIdx = 1;

}

//-----------------------------------------------------------------------------

int Pit::GetWidth() {
  return width;
}

int Pit::GetHeight() {
  return height;
}

int Pit::GetDepth() {
  return depth;
}

//-----------------------------------------------------------------------------

VERTEX Pit::GetOrigin() {

  return origin;

}

//-----------------------------------------------------------------------------

float Pit::GetCubeSide() {

  return cubeSide;

}

//-----------------------------------------------------------------------------

void Pit::GetOutOfBounds(int x,int y,int z,int *ox,int *oy,int *oz) {

  *ox = 0;
  *oy = 0;
  *oz = 0;

  if( x<0 )
    *ox = -x;

  if( x>=width )
    *ox = width - x - 1;

  if( y<0 )
    *oy = -y;

  if( y>=height )
    *oy = height - y - 1;

  if( z<0 )
    *oz = -z;

  if( z>=depth )
    *oz = depth - z - 1;

}

//-----------------------------------------------------------------------------

int Pit::GetValue(int x,int y,int z) {

  if( x<0 || x>=width || y<0 || y>=height || z<0 || z>=depth )
    return 1;
  else
    return matrix[x + y*width + z*width*height];

}

//-----------------------------------------------------------------------------

int Pit::GetValue2(int x,int y,int z) {

  if( x<0 || x>=width || y<0 || y>=height || z<0 || z>=depth )
    return 0;
  else
    return matrix[x + y*width + z*width*height];

}

//-----------------------------------------------------------------------------

BOOL Pit::IsLineFull(int z) {

  BOOL full = TRUE;
  for(int i=0;i<width && full;i++)
    for(int j=0;j<height && full;j++)
      full = full && (GetValue(i,j,z)>=1);
  return full;

}

//-----------------------------------------------------------------------------

BOOL Pit::IsLineEmpty(int z) {

  BOOL empty = TRUE;
  for(int i=0;i<width && empty;i++)
    for(int j=0;j<height && empty;j++)
      empty = empty && (GetValue(i,j,z)==0);
  return empty;

}

//-----------------------------------------------------------------------------

void Pit::SetValue(int x,int y,int z,int value) {

  if( x>=0 && x<width && y>=0 && y<height && z>=0 && z<depth ) {
    matrix[x + y*width + z*width*height] = value;
  }

}

//-----------------------------------------------------------------------------

void Pit::AddCube(int x,int y,int z) {

  tcubeIdx++;
  if(tcubeIdx>NBTCUBE) tcubeIdx=1;
  SetValue(x,y,z,tcubeIdx);

}

//-----------------------------------------------------------------------------

void Pit::RenderEdge(int x,int y,int z,int edge) {

  BOOL isGreen=FALSE;

  if( style==STYLE_CLASSIC ) {

  switch(edge) {
    case 0:
      isGreen = (y==height-1);
      break;
    case 1:
      isGreen = (x==width-1);
      break;
    case 2:
      isGreen = (y==0);
      break;
    case 3:
      isGreen = (x==0);
      break;
    case 4:
      isGreen = (x==0) || (y==height-1);
      break;
    case 5:
      isGreen = (x==width-1) || (y==height-1);
      break;
    case 6:
      isGreen = (x==width-1) || (y==0);
      break;
    case 7:
      isGreen = (x==0) || (y==0);
      break;
    case 8:
    case 9:
    case 10:
    case 11:
      isGreen = (z==depth-1);
      break;
  }

  }

  if( isGreen )
    GLApplication::SetMaterial( &gridMaterial );
  else
    GLApplication::SetMaterial( &blackMaterial );

  glCallList(lcubeList[edge]);

}

//-----------------------------------------------------------------------------

void Pit::RenderCube(int x,int y,int z) {

  glTranslatef(x*cubeSide,y*cubeSide,z*cubeSide);

  int mX = width/2;
  int mY = height/2;

  switch(style) {

  case STYLE_CLASSIC:

  // Draw the cube
  GLApplication::SetMaterial( GetMaterial(z) );
  glCallList(cubeList);

  // Draw the contour

  // Up
  RenderEdge(x,y,z,0);
  RenderEdge(x,y,z,1);
  RenderEdge(x,y,z,2);
  RenderEdge(x,y,z,3);

  // East
  if( x<mX ) {
    RenderEdge(x,y,z,5);
    RenderEdge(x,y,z,9);
    RenderEdge(x,y,z,6);
  }

  // West
  if( x>mX ) {
    RenderEdge(x,y,z,4);
    RenderEdge(x,y,z,11);
    RenderEdge(x,y,z,7);
  }

  // North
  if( y<mY ) {
    RenderEdge(x,y,z,4);
    RenderEdge(x,y,z,8);
    RenderEdge(x,y,z,5);
  }

  // South
  if( y>mY ) {
    RenderEdge(x,y,z,7);
    RenderEdge(x,y,z,10);
    RenderEdge(x,y,z,6);
  }
  break;

  case STYLE_MARBLE:
  case STYLE_ARCADE:

  // Enable texture
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,cubeTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Draw the cube
  GLApplication::SetMaterial( GetMaterial(z) );
  glCallList(cubeTList[GetValue(x,y,z)-1]);
    
  // Disable texture
  glDisable(GL_TEXTURE_2D);
  break;
  
  }  // End switch(style)

  return;

}

//-----------------------------------------------------------------------------

void Pit::SetPix(int x,int y,GLMATERIAL *mat) {

  int add = 4 * (pitLevelW*(pitLevelH-y-1) +  x);
  pitLevelSurf[add+0] = mat->Ambient.r;
  pitLevelSurf[add+1] = mat->Ambient.g;
  pitLevelSurf[add+2] = mat->Ambient.b;
  pitLevelSurf[add+3] = 1.0f;

}

//-----------------------------------------------------------------------------

void Pit::RenderLevel() {

   if( !inited ) return;

   if( pitLevelW < 8 ) {
     // TODO small pit level
     return;
   }

   // Clear
   memset(pitLevelSurf,0,pitLevelW * pitLevelH * 4 * sizeof(GLfloat));

   // Draw the pit level

   int cS = (pitLevelH-2) / 20;
   int sY = pitLevelH - (cS * depth) - 2;

   for(int j=0;j<depth;j++) {

     // Border

     SetPix(1,sY,&gridMaterial);
     SetPix(2,sY,&gridMaterial);
     SetPix(pitLevelW-3,sY,&gridMaterial);
     SetPix(pitLevelW-2,sY,&gridMaterial);
     for(int i=sY;i<sY+cS;i++) {
       SetPix(0,i,&gridMaterial);
       SetPix(pitLevelW-1,i,&gridMaterial);
     }

     // Cube
     if( !IsLineEmpty(j) ) {
       DrawPitLevelCubes(3,sY+2,pitLevelW-6,cS-3,GetMaterial(j));
     }

     sY += cS;
   }

   for(int i=0;i<pitLevelW;i++) {
     SetPix(i,pitLevelH-2,&gridMaterial);
   }

#ifndef PLATFORM_PSP
   glRasterPos2i(pitLevelX,pitLevelY+pitLevelH);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_BLEND);
   glDisable(GL_DEPTH_TEST);
   glDisable(GL_LIGHTING);
   glDisable(GL_TEXTURE_2D);
   glDrawPixels(pitLevelW,pitLevelH,GL_RGBA,GL_FLOAT,pitLevelSurf);
#endif
}

//-----------------------------------------------------------------------------

void Pit::DrawPitLevelCubes(int x,int y,int w,int h,GLMATERIAL *mat) {

  DWORD white = 0x909090;

  // Cubes coordinates
  int f1 = fround( (float)h/4.0f );
  int f2 = fround( (float)h/2.0f );
  int wc = w/3;
  int sX = h/2;
  int eX = sX+2*wc;

  // ----- Filling

  for(int j=0;j<h;j++) {

    if( j>f2 ) eX--;

    for(int i=sX;i<eX;i++)
      SetPix(i+x,j+y,mat);

    sX--;
    if( sX<0 ) sX=0;

  }

  // ----- Side
  sX = h/2;

  for(int j=0;j<h;j++) {
    
    if( j==0 || j==f1 || j==f2 || j==h-1 ) {

      // Plein green line
      for(int i=sX;i<=sX+2*wc;i++)
        SetPix(i+x,j+y,&whiteMaterial);

    } else {

      // Dotted line      
      SetPix(sX+x,j+y,&whiteMaterial);
      SetPix(sX+wc+x,j+y,&whiteMaterial);
      SetPix(sX+2*wc+x,j+y,&whiteMaterial);
    }

    // South east side
    if( sX>0 ) {
      SetPix(sX+x+2*wc,j+y+h/2,&whiteMaterial);
    }
    // End verticals
    if( j>=0 && j<=h/2 ) {
      SetPix(h/2 + 2*wc + x,j+y,&whiteMaterial);
    }
    if( j>=f1 && j<=f1+h/2 ) {
      SetPix(h/2 - f1 + 2*wc + x,j+y,&whiteMaterial);
    }

    sX--;
    if( sX<0 ) sX=0;

  }

}

//-----------------------------------------------------------------------------

void Pit::Render(BOOL renderCube,BOOL zBuffer) {

  if( !inited ) return;

  // Disable texture, alpha
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  if( zBuffer )
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);

  switch(style) {

  case STYLE_CLASSIC:

  // Draw side
  if( !zBuffer ) {
#ifndef PLATFORM_PSP
    glDisable(GL_LIGHTING);
    GLApplication::SetMaterial (&blackMaterial);    
    glCallList(sideList);
#endif

    // Draw the back
    glEnable(GL_LIGHTING);
    GLApplication::SetMaterial (&backMaterial);    
    glCallList(backList);

  }

  // Draw the grid
  glDisable(GL_LIGHTING);
  GLApplication::SetMaterial (&gridMaterial);    
  glCallList(gridList);
  break;

  case STYLE_MARBLE:

  // Draw side
  if( !zBuffer ) {
    glDisable(GL_LIGHTING);
    GLApplication::SetMaterial (&blackMaterial);    
    glCallList(sideList);
  }

  // Enable texture
  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,backTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Draw the back
  GLApplication::SetMaterial (&backTexMaterial);    
  glCallList(backList);

  // Disable texture
  glDisable(GL_TEXTURE_2D);

  // Draw the grid (Horizontal line)
  if( !zBuffer ) {
    glDisable(GL_LIGHTING);
    GLApplication::SetMaterial (&darkMaterial);    
    glCallList(hGridList);
  }
  break;

  case STYLE_ARCADE:

  // Draw the grid
  GLApplication::SetMaterial (&darkMaterial);    
  glCallList(gridList);

  break;
  
  } // End switch(style)

  // Draw filling cubes
  if( renderCube ) {
    for(int i=0;i<mSize;i++) {
      int x = orderMatrix[i].x;
      int y = orderMatrix[i].y;
      int z = orderMatrix[i].z;
      if( GetValue(x,y,z) ) {
        glPushMatrix();
        if( zBuffer ) {
          if( IsVisible2(x,y,z) ) 
            RenderCube(x,y,z);
        } else {
          if( IsVisible(x,y,z) ) 
            RenderCube(x,y,z);
        }
        glPopMatrix();
      }
    }
  }

}

//-----------------------------------------------------------------------------

void Pit::InvalidateDeviceObjects() {

    DELETE_TEX(backTexture);
    DELETE_TEX(cubeTexture);

     DELETE_LIST(gridList);
     DELETE_LIST(cubeList);
     DELETE_LIST(backList);
    DELETE_LIST(sideList);
    DELETE_LIST(hGridList);

    for(int i=0;i<NBTCUBE;i++)
      DELETE_LIST(cubeTList[i]);

    for(int i=0;i<12;i++)
      DELETE_LIST(lcubeList[i]);

    if( pitLevelSurf ) {
      free(pitLevelSurf);
      pitLevelSurf = NULL;
    }

    inited = FALSE;

}

#ifdef PLATFORM_PSP
int Pit::GetLevel() {
    int level = 0;

    for(int i=0;i<depth;i++) {
        if( !IsLineEmpty(i) )
            level++;
    }

    return level;
}

#endif