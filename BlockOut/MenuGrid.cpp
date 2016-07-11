/*
   File:        MenuGrid.cpp
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

#include "MenuGrid.h"

// ----------------------------------------------------------------

MenuGrid::MenuGrid() {

  gridList = 0;
  redList = 0;
  blueList = 0;
  topList = 0;
  nwList = 0;
  neList = 0;
  swList = 0;
  seList = 0;
  bnList = 0;
  bwList = 0;
  bsList = 0;
  beList = 0;
  memset(matrix,0,sizeof(matrix));

  // Initialiase order matrix
  int i,j;
  int dist[GRID_WIDTH*GRID_HEIGHT];
  for(i=0;i<GRID_WIDTH;i++) {
    for(j=0;j<GRID_HEIGHT;j++) {
      orderMatrix[i + GRID_WIDTH*j].x = i;
      orderMatrix[i + GRID_WIDTH*j].y = j;
      dist[i + GRID_WIDTH*j] = (i-GRID_WIDTH/2) * (i-GRID_WIDTH/2) + (j-GRID_HEIGHT/2) * (j-GRID_HEIGHT/2);
    }
  }

  // Sort
  BOOL end = FALSE;
  i=0;
  j=GRID_WIDTH*GRID_HEIGHT-1;

  while(!end) {
    end = TRUE;
    for(i=0;i<j;i++) {
      if( dist[i]<dist[i+1] ) {

        // Swap dist
        int tmp = dist[i];
        dist[i] = dist[i+1];
        dist[i+1] = tmp;
        // Swap orderMatrix
        POINT2D tmp2 = orderMatrix[i];
        orderMatrix[i] = orderMatrix[i+1];
        orderMatrix[i+1] = tmp2;

        end = FALSE;
      }
    }
    j--;
  }

}

// ----------------------------------------------------------------

void MenuGrid::Clear() {
  memset(matrix,0,sizeof(matrix));
}

// ----------------------------------------------------------------

void MenuGrid::SetValue(int x,int y,int value) {
  matrix[x + GRID_WIDTH*y] = value;
}

// ----------------------------------------------------------------

int MenuGrid::Create() {

  // Global var
  cSide  = 0.1f;
  startX = -(float)GRID_WIDTH  * cSide / 2.0f;
  startY = -(float)GRID_HEIGHT * cSide / 2.0f;
  startZ = 3.0f + cSide;

  if( !CreateGrid() ) return GL_FAIL;
  if( !CreateCube() ) return GL_FAIL;

  return GL_OK;

}

// ----------------------------------------------------------------

int MenuGrid::CreateGrid() {

  // Material
  memset (&gridMaterial, 0, sizeof (GLMATERIAL));
  gridMaterial.Diffuse.r = 1.0f;
  gridMaterial.Diffuse.g = 1.0f;
  gridMaterial.Diffuse.b = 1.0f;
  gridMaterial.Ambient.r = 0.0f;
  gridMaterial.Ambient.g = 0.5f;
  gridMaterial.Ambient.b = 0.0f;

  // Grid
  gridList = glGenLists(1);
  glNewList(gridList,GL_COMPILE);
  glBegin(GL_LINES);
  for(int i=0;i<=GRID_WIDTH;i++) {
    glVertex3f(startX + (i*cSide) - cSide/2.0f, -startY , startZ);
    glVertex3f(startX + (i*cSide) - cSide/2.0f,  startY , startZ);
  }
  for(int i=0;i<=GRID_HEIGHT;i++) {
    glVertex3f( -startX - cSide/2.0f, startY + (i*cSide) , startZ );
    glVertex3f(  startX - cSide/2.0f, startY + (i*cSide) , startZ );
  }
  glEnd();
  glEndList();

  return GL_OK;

}

// ----------------------------------------------------------------

int MenuGrid::CreateCube() {

  // Material
  memset (&redMaterial, 0, sizeof (GLMATERIAL));
  redMaterial.Diffuse.r = 0.5f;
  redMaterial.Diffuse.g = 0.0f;
  redMaterial.Diffuse.b = 0.0f;
  redMaterial.Ambient.r = 1.0f;
  redMaterial.Ambient.g = 0.0f;
  redMaterial.Ambient.b = 0.0f;

  // Red face
  redList = glGenLists(1);
  glNewList(redList,GL_COMPILE);
  glBegin(GL_QUADS);

  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f(0 * cSide + startX,
             1 * cSide + startY,
            -1 * cSide + startZ);

  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f( 1 * cSide + startX,
              1 * cSide + startY,
             -1 * cSide + startZ);

  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f( 1 * cSide + startX,
              0 * cSide + startY,
             -1 * cSide + startZ);

  glNormal3f(0.0f,0.0f,-1.0f);
  glVertex3f( 0 * cSide + startX,
              0 * cSide + startY,
             -1 * cSide + startZ );
  glEnd();
  glEndList();

  // Material
  memset (&blueMaterial, 0, sizeof (GLMATERIAL));
  blueMaterial.Diffuse.r = 0.0f;
  blueMaterial.Diffuse.g = 0.0f;
  blueMaterial.Diffuse.b = 1.0f;
  blueMaterial.Ambient.r = 0.2f;
  blueMaterial.Ambient.g = 0.2f;
  blueMaterial.Ambient.b = 0.7f;

  // Blue faces
  blueList = glGenLists(1);
  glNewList(blueList,GL_COMPILE);
  glBegin(GL_QUADS);

  // Face 1
  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f( 1 * cSide + startX,
              1 * cSide + startY,
             -1 * cSide + startZ);

  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f( 1 * cSide + startX,
              1 * cSide + startY,
              0 * cSide + startZ);

  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f( 1 * cSide + startX,
              0 * cSide + startY,
              0 * cSide + startZ);

  glNormal3f(1.0f,0.0f,0.0f);
  glVertex3f( 1 * cSide + startX,
              0 * cSide + startY,
             -1 * cSide + startZ);

  // Face 2
  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f( 0 * cSide + startX,
              1 * cSide + startY,
              0 * cSide + startZ);

  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f( 1 * cSide + startX,
              1 * cSide + startY,
              0 * cSide + startZ);

  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f( 1 * cSide + startX,
              1 * cSide + startY,
             -1 * cSide + startZ);

  glNormal3f(0.0f,1.0f,0.0f);
  glVertex3f( 0 * cSide + startX,
              1 * cSide + startY,
             -1 * cSide + startZ);

  // Face 3
  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f( 0 * cSide + startX,
              1 * cSide + startY,
              0 * cSide + startZ);

  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f( 0 * cSide + startX,
              1 * cSide + startY,
             -1 * cSide + startZ);

  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f( 0 * cSide + startX,
              0 * cSide + startY,
             -1 * cSide + startZ);

  glNormal3f(-1.0f,0.0f,0.0f);
  glVertex3f( 0 * cSide + startX,
              0 * cSide + startY,
              0 * cSide + startZ);

  // Face 4
  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f( 1 * cSide + startX,
              0 * cSide + startY,
             -1 * cSide + startZ);

  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f( 1 * cSide + startX,
              0 * cSide + startY,
              0 * cSide + startZ);

  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f( 0 * cSide + startX,
              0 * cSide + startY,
              0 * cSide + startZ);

  glNormal3f(0.0f,-1.0f,0.0f);
  glVertex3f( 0 * cSide + startX,
              0 * cSide + startY,
             -1 * cSide + startZ);

  glEnd();
  glEndList();

  // Edge
  topList = glGenLists(1);
  glNewList(topList,GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f( 0 * cSide + startX,1 * cSide + startY,-1 * cSide + startZ);  // 0
  glVertex3f( 1 * cSide + startX,1 * cSide + startY,-1 * cSide + startZ);  // 1

  glVertex3f( 1 * cSide + startX,1 * cSide + startY,-1 * cSide + startZ);  // 1
  glVertex3f( 1 * cSide + startX,0 * cSide + startY,-1 * cSide + startZ);  // 2

  glVertex3f( 1 * cSide + startX,0 * cSide + startY,-1 * cSide + startZ);  // 2
  glVertex3f( 0 * cSide + startX,0 * cSide + startY,-1 * cSide + startZ);  // 3

  glVertex3f( 0 * cSide + startX,0 * cSide + startY,-1 * cSide + startZ);  // 3
  glVertex3f( 0 * cSide + startX,1 * cSide + startY,-1 * cSide + startZ);  // 0
  glEnd();
  glEndList();

  nwList = glGenLists(1);
  glNewList(nwList,GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f( 1 * cSide + startX,1 * cSide + startY,-1 * cSide + startZ);  // 1
  glVertex3f( 1 * cSide + startX,1 * cSide + startY,0 * cSide + startZ);   // 5
  glEnd();
  glEndList();

  neList = glGenLists(1);
  glNewList(neList,GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f( 0 * cSide + startX,1 * cSide + startY,-1 * cSide + startZ);  // 0
  glVertex3f( 0 * cSide + startX,1 * cSide + startY,0 * cSide + startZ);   // 4
  glEnd();
  glEndList();

  swList = glGenLists(1);
  glNewList(swList,GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f( 1 * cSide + startX,0 * cSide + startY,-1 * cSide + startZ);  // 2
  glVertex3f( 1 * cSide + startX,0 * cSide + startY,0 * cSide + startZ);   // 6
  glEnd();
  glEndList();

  seList = glGenLists(1);
  glNewList(seList,GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f( 0 * cSide + startX,0 * cSide + startY,-1 * cSide + startZ);  // 3
  glVertex3f( 0 * cSide + startX,0 * cSide + startY,0 * cSide + startZ);   // 7
  glEnd();
  glEndList();

  bnList = glGenLists(1);
  glNewList(bnList,GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f( 0 * cSide + startX,1 * cSide + startY,0 * cSide + startZ);   // 4
  glVertex3f( 1 * cSide + startX,1 * cSide + startY,0 * cSide + startZ);   // 5
  glEnd();
  glEndList();

  bwList = glGenLists(1);
  glNewList(bwList,GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f( 1 * cSide + startX,1 * cSide + startY,0 * cSide + startZ);   // 5
  glVertex3f( 1 * cSide + startX,0 * cSide + startY,0 * cSide + startZ);   // 6
  glEnd();
  glEndList();

  bsList = glGenLists(1);
  glNewList(bsList,GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f( 1 * cSide + startX,0 * cSide + startY,0 * cSide + startZ);   // 6
  glVertex3f( 0 * cSide + startX,0 * cSide + startY,0 * cSide + startZ);   // 7
  glEnd();
  glEndList();

  beList = glGenLists(1);
  glNewList(beList,GL_COMPILE);
  glBegin(GL_LINES);
  glVertex3f( 0 * cSide + startX,0 * cSide + startY,0 * cSide + startZ);   // 7
  glVertex3f( 0 * cSide + startX,1 * cSide + startY,0 * cSide + startZ);   // 4
  glEnd();
  glEndList();

  return GL_OK;
}

// ----------------------------------------------------------------

void MenuGrid::RenderCube(int x,int y) {

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_LIGHTING);
  glTranslatef(x*cSide - cSide/2.0f,y*cSide,0.0f);

  // Draw faces
  GLApplication::SetMaterial(&blueMaterial);
  glCallList(blueList);
  GLApplication::SetMaterial(&redMaterial);
  glCallList(redList);

  // Edges
  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);

  // Top edge
  GLApplication::SetMaterial( &gridMaterial );
  glCallList(topList);

  if( x<GRID_WIDTH/2 || y<GRID_HEIGHT/2) {
    glCallList(nwList);
  }

  if( x>GRID_WIDTH/2 || y<GRID_HEIGHT/2) {
    glCallList(neList);
  }

  if( x<GRID_WIDTH/2 || y>GRID_HEIGHT/2) {
    glCallList(swList);
  }

  if( x>GRID_WIDTH/2 || y>GRID_HEIGHT/2) {
    glCallList(seList);
  }

  if( x>GRID_WIDTH/2 ) {
    glCallList(beList);
  } else {
    glCallList(bwList);
  }

  if( y>GRID_HEIGHT/2 ) {
    glCallList(bsList);
  } else {
    glCallList(bnList);
  }

}

// ----------------------------------------------------------------

void MenuGrid::Render() {

  // Render grid
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glLineWidth(1.0f);

  GLApplication::SetMaterial(&gridMaterial);
  glCallList(gridList);

  // Render cubes
  for(int i=0;i<GRID_WIDTH*GRID_HEIGHT;i++) {
    int x = orderMatrix[i].x;
    int y = orderMatrix[i].y;
    glPushMatrix();
    if( matrix[x + GRID_WIDTH*y] ) {
      RenderCube(x,y);
    }
    glPopMatrix();
  }

}

// ----------------------------------------------------------------

void MenuGrid::InvalidateDeviceObjects() {

     DELETE_LIST(gridList);
    DELETE_LIST(blueList);
    DELETE_LIST(redList);
     DELETE_LIST(topList);
     DELETE_LIST(nwList);
     DELETE_LIST(neList);
     DELETE_LIST(swList);
     DELETE_LIST(seList);
     DELETE_LIST(bnList);
     DELETE_LIST(bwList);
     DELETE_LIST(bsList);
     DELETE_LIST(beList);

}

