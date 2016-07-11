/*
  File:        PolyCube.cpp
  Description: PolyCube management
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
#include <math.h>

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#define DIR_OX 1
#define DIR_OY 2
#define DIR_OZ 3

const EDGE edgeOrg[] = { 

  {{0,1,0} , {1,1,0} , DIR_OX} , // 0
  {{1,1,0} , {1,0,0} , DIR_OY} , // 1
  {{0,0,0} , {1,0,0} , DIR_OX} , // 2
  {{0,1,0} , {0,0,0} , DIR_OY} , // 3

  {{0,1,0} , {0,1,1} , DIR_OZ} , // 4
  {{1,1,0} , {1,1,1} , DIR_OZ} , // 5
  {{1,0,0} , {1,0,1} , DIR_OZ} , // 6
  {{0,0,0} , {0,0,1} , DIR_OZ} , // 7

  {{0,1,1} , {1,1,1} , DIR_OX} , // 8
  {{1,1,1} , {1,0,1} , DIR_OY} , // 9
  {{0,0,1} , {1,0,1} , DIR_OX} , // 10
  {{0,1,1} , {0,0,1} , DIR_OY}   // 11

};

//-----------------------------------------------------------------------------

PolyCube::PolyCube() {

  lineList  = 0;
  ghostList = 0;
  bigEdgeList = 0;
  nbCube = 0;
  hScore = 0;
  lScore = 0;  
  isFlat = FALSE;
  isBasic = FALSE;
  hasGhost = FALSE;
  nbOrientation = 0;
  hasGhost = FALSE;
  edges = (EDGE *)malloc(MAX_CUBE*12*sizeof(EDGE));
  allRot = (ORIENTATION *)malloc(24*sizeof(ORIENTATION));

}

//-----------------------------------------------------------------------------

PolyCube::~PolyCube() {
  free(edges);
  free(allRot);
}

//-----------------------------------------------------------------------------

void PolyCube::SetInfo(int highScore,int lowScore,BOOL flat,BOOL basic) {

  hScore = highScore;
  lScore = lowScore;
  isFlat = flat;
  isBasic = basic;

}

//-----------------------------------------------------------------------------

void PolyCube::AddCube(int x,int y,int z) {

  if( nbCube<MAX_CUBE ) {
    cubes[nbCube].x = x;
    cubes[nbCube].y = y;
    cubes[nbCube].z = z;
    nbCube++;
  }

}

//-----------------------------------------------------------------------------

void PolyCube::AddOrientation(int r0,int r1,int r2) {
  allRot[nbOrientation].r0 = r0;
  allRot[nbOrientation].r1 = r1;
  allRot[nbOrientation].r2 = r2;
  nbOrientation++;
}

//-----------------------------------------------------------------------------

ORIENTATION *PolyCube::GetOrientationAt(int idx) {
  return &(allRot[idx]);
}

//-----------------------------------------------------------------------------
int PolyCube::GetNbOrientation() {
  return nbOrientation;
}

//-----------------------------------------------------------------------------

int PolyCube::GetHighScore() {

  return hScore;

}

//-----------------------------------------------------------------------------

int PolyCube::GetLowScore() {

  return lScore;

}

//-----------------------------------------------------------------------------

BOOL PolyCube::IsInSet(int set) {

  switch(set) {
    case BLOCKSET_EXTENDED:
      return TRUE;
    case BLOCKSET_BASIC:
      return isBasic;
    case BLOCKSET_FLAT:
      return isFlat;
  }

  return FALSE;

}

//-----------------------------------------------------------------------------

int PolyCube::GetNbCube() {

  return nbCube;

}

//-----------------------------------------------------------------------------

int PolyCube::GetWidth() {
  int maxW = 0;
  for(int i=0;i<nbCube;i++)
    if(cubes[i].x > maxW) maxW = cubes[i].x;
  return maxW + 1;
}

//-----------------------------------------------------------------------------

int PolyCube::GetHeight() {
  int maxH = 0;
  for(int i=0;i<nbCube;i++)
    if(cubes[i].y > maxH) maxH = cubes[i].y;
  return maxH + 1;
}

//-----------------------------------------------------------------------------

int PolyCube::GetDepth() {
  int maxD = 0;
  for(int i=0;i<nbCube;i++)
    if(cubes[i].z > maxD) maxD = cubes[i].z;
  return maxD + 1;
}

//-----------------------------------------------------------------------------

int PolyCube::GetMaxDim() {
  int w = GetWidth();
  int h = GetHeight();
  int d = GetDepth();
  int maxDim = w;
  if( h > maxDim ) maxDim = h;
  if( d > maxDim ) maxDim = d;
  return maxDim;
}

//-----------------------------------------------------------------------------

void PolyCube::InitRotationCenter() {

  // Emulate "BlockOut original" rotation center
  iCenter.x = GetWidth() - 1;
  iCenter.y = 1;
  iCenter.z = GetDepth() - 1;

  center.x = iCenter.x * cubeSide + origin.x;
  center.y = iCenter.y * cubeSide + origin.y;
  center.z = iCenter.z * cubeSide + origin.z;

}
//-----------------------------------------------------------------------------

BOOL PolyCube::EdgeExist(EDGE e) {

  BOOL found = FALSE;
  int i = 0;
  while(i<nbEdge && !found) {
    found = EdgeEqual(edges[i],e);
    if(!found) i++;
  }
  return found;

}

//-----------------------------------------------------------------------------

void PolyCube::GetEdgesAtCorner(int x,int y,int z,int *nb,int *direction) {

  int vx,vy,vz;

  for(int i=0;i<nbEdge;i++) {

    EDGE e = edges[i];
    vx=0; vy=0; vz=0;

    if( (e.p1.x == x && e.p1.y == y && e.p1.z == z) ) {
      vx = e.p1.x - e.p2.x;
      vy = e.p1.y - e.p2.y;
      vz = e.p1.z - e.p2.z;
    }

    if( (e.p2.x == x && e.p2.y == y && e.p2.z == z) ) {
      vx = e.p2.x - e.p1.x;
      vy = e.p2.y - e.p1.y;
      vz = e.p2.z - e.p1.z;
    }

    if( vx ) {
      if( vx<0 ) direction[*nb] = -DIR_OX;
      else       direction[*nb] =  DIR_OX;
      *nb=*nb+1;
    } else if( vy ) {
      if( vy<0 ) direction[*nb] = -DIR_OY;
      else       direction[*nb] =  DIR_OY;
      *nb=*nb+1;
    } else if( vz ) {
      if( vz<0 ) direction[*nb] = -DIR_OZ;
      else       direction[*nb] =  DIR_OZ;
      *nb=*nb+1;
    }

  }

}

//-----------------------------------------------------------------------------

BOOL PolyCube::IsPermut(int d1,int d2,int d3,int *dir) {

  return (d1==dir[0] && d2==dir[1] && d3==dir[2]) ||
         (d1==dir[0] && d2==dir[2] && d3==dir[1]) ||
         (d1==dir[1] && d2==dir[0] && d3==dir[2]) ||
         (d1==dir[1] && d2==dir[2] && d3==dir[0]) ||
         (d1==dir[2] && d2==dir[1] && d3==dir[0]) ||
         (d1==dir[2] && d2==dir[0] && d3==dir[1]);

}

//-----------------------------------------------------------------------------

BOOL PolyCube::GetCornerAt(int x,int y,int z,VERTEX *o) {

    int direction[8];  // 4 should be the max
    int nbDir=0;

    GetEdgesAtCorner(x,y,z,&nbDir,direction);
    if( nbDir==3 ) {
      if( IsPermut(-DIR_OX,+DIR_OY,-DIR_OZ,direction) ) {
        o->x = -1.0f; o->y = 1.0f; o->z = -1.0f;
        return TRUE;
      } else if( IsPermut(+DIR_OX,+DIR_OY,-DIR_OZ,direction) ) {
        o->x = 1.0f; o->y = 1.0f; o->z = -1.0f;
        return TRUE;
      } else if( IsPermut(+DIR_OX,-DIR_OY,-DIR_OZ,direction) ) {
        o->x = 1.0f; o->y = -1.0f; o->z = -1.0f;
        return TRUE;
      } else if( IsPermut(-DIR_OX,-DIR_OY,-DIR_OZ,direction) ) {
        o->x = -1.0f; o->y = -1.0f; o->z = -1.0f;
        return TRUE;
      } else if( IsPermut(-DIR_OX,+DIR_OY,+DIR_OZ,direction) ) {
        o->x = -1.0f; o->y = 1.0f; o->z = 1.0f;
        return TRUE;
      } else if( IsPermut(+DIR_OX,+DIR_OY,+DIR_OZ,direction) ) {
        o->x = 1.0f; o->y = 1.0f; o->z = 1.0f;
        return TRUE;
      } else if( IsPermut(+DIR_OX,-DIR_OY,+DIR_OZ,direction) ) {
        o->x = 1.0f; o->y = -1.0f; o->z = 1.0f;
        return TRUE;
      } else if( IsPermut(-DIR_OX,-DIR_OY,+DIR_OZ,direction) ) {
        o->x = -1.0f; o->y = -1.0f; o->z = 1.0f;
        return TRUE;
      }
    }

    return FALSE;

}

//-----------------------------------------------------------------------------

BOOL PolyCube::EdgeEqual(EDGE e1,EDGE e2) {

  return ((e1.p1.x == e2.p1.x && e1.p1.y == e2.p1.y && e1.p1.z == e2.p1.z) &&
          (e1.p2.x == e2.p2.x && e1.p2.y == e2.p2.y && e1.p2.z == e2.p2.z) ) ||
         ((e1.p1.x == e2.p2.x && e1.p1.y == e2.p2.y && e1.p1.z == e2.p2.z) &&
          (e1.p2.x == e2.p1.x && e1.p2.y == e2.p1.y && e1.p2.z == e2.p1.z) );

}

//-----------------------------------------------------------------------------

int PolyCube::Create(float cSide,VERTEX org,int ghost,float wEdge) {

  int hr;

  cubeSide = cSide;
  origin = org;
  hasBigEdge = (wEdge!=0.0f);

  // Create edge

  nbEdge = 0;
  for(int i=0;i<nbCube;i++) {
    for(int j=0;j<12;j++) {
      if( IsEdgeVisible(i,j) ) {

        EDGE e;
        e.p1.x = cubes[i].x + edgeOrg[j].p1.x;
        e.p1.y = cubes[i].y + edgeOrg[j].p1.y;
        e.p1.z = cubes[i].z + edgeOrg[j].p1.z;

        e.p2.x = cubes[i].x + edgeOrg[j].p2.x;
        e.p2.y = cubes[i].y + edgeOrg[j].p2.y;
        e.p2.z = cubes[i].z + edgeOrg[j].p2.z;

        e.orientation = edgeOrg[j].orientation;

        if(!EdgeExist(e)) edges[nbEdge++] = e;

      }
    }
  }

  if(!hasBigEdge) {
    hr = CreateLineEdge();
    if(!hr) return GL_FAIL;
  } else {
    hr = CreateCylinderEdges(wEdge*cubeSide);
    if(!hr) return GL_FAIL;
  }


  // Init material
  memset (&whiteMaterial, 0, sizeof (GLMATERIAL));
  whiteMaterial.Diffuse.r = 1.0f;
  whiteMaterial.Diffuse.g = 1.0f;
  whiteMaterial.Diffuse.b = 1.0f;
  whiteMaterial.Ambient.r = 1.0f;
  whiteMaterial.Ambient.g = 1.0f;
  whiteMaterial.Ambient.b = 1.0f;

  memset (&redMaterial, 0, sizeof (GLMATERIAL));
  redMaterial.Diffuse.r = 1.0f;
  redMaterial.Diffuse.g = 0.0f;
  redMaterial.Diffuse.b = 0.0f;
  redMaterial.Ambient.r = 1.0f;
  redMaterial.Ambient.g = 0.0f;
  redMaterial.Ambient.b = 0.0f;

  memset (&ghostMaterial, 0, sizeof (GLMATERIAL));

  memset (&grayMaterial, 0, sizeof (GLMATERIAL));
  grayMaterial.Diffuse.r = 0.8f;
  grayMaterial.Diffuse.g = 0.8f;
  grayMaterial.Diffuse.b = 0.8f;
  grayMaterial.Ambient.r = 0.5f;
  grayMaterial.Ambient.g = 0.5f;
  grayMaterial.Ambient.b = 0.5f;
  grayMaterial.Specular.r = 1.0f;
  grayMaterial.Specular.g = 1.0f;
  grayMaterial.Specular.b = 1.0f;
  grayMaterial.Power = 20.0f;

  InitRotationCenter();

  // Ghost
  hasGhost = (ghost!=0);
  if( hasGhost ) {
    if( !CreateGhost(ghost) ) return GL_FAIL;
  }

  return GL_OK;
}

//-----------------------------------------------------------------------------

int PolyCube::CreateLineEdge() {

  lineList = glGenLists(1);
  glNewList(lineList,GL_COMPILE);
  glBegin(GL_LINES);

  for(int i=0;i<nbEdge;i++) {

    glVertex3f( (float)(edges[i].p1.x) * cubeSide + origin.x,
                (float)(edges[i].p1.y) * cubeSide + origin.y,
                (float)(edges[i].p1.z) * cubeSide + origin.z);

    glVertex3f( (float)(edges[i].p2.x) * cubeSide + origin.x,
                (float)(edges[i].p2.y) * cubeSide + origin.y,
                (float)(edges[i].p2.z) * cubeSide + origin.z);

  }

  glEnd();
  glEndList();

  return GL_OK;
}

//-----------------------------------------------------------------------------

int PolyCube::CreateCylinderEdges(float wEdge) {

  CORNER       corner[40];
  int          nbCorner=0;
  int          nbDiv = 6;
  VERTEX       oV;

  // Compute corner, node where 3 edges intersect
  for(int i=0;i<=GetWidth();i++) {
    for(int j=0;j<=GetHeight();j++) {
      for(int k=0;k<=GetDepth();k++) {
        if( GetCornerAt(i,j,k,&oV) ) {
          corner[nbCorner].p.x = (float)i * cubeSide + origin.x;
          corner[nbCorner].p.y = (float)j * cubeSide + origin.y;
          corner[nbCorner].p.z = (float)k * cubeSide + origin.z;
          corner[nbCorner].o = oV;
          nbCorner++;
        }
      }
    }
  }

  bigEdgeList = glGenLists(1);
  glNewList(bigEdgeList,GL_COMPILE);

  // Edge cylinder
  glBegin(GL_QUADS);

  for(int i=0;i<nbEdge;i++) {

    float x1c = (float)(edges[i].p1.x) * cubeSide + origin.x;
    float y1c = (float)(edges[i].p1.y) * cubeSide + origin.y;
    float z1c = (float)(edges[i].p1.z) * cubeSide + origin.z;

    float x2c = (float)(edges[i].p2.x) * cubeSide + origin.x;
    float y2c = (float)(edges[i].p2.y) * cubeSide + origin.y;
    float z2c = (float)(edges[i].p2.z) * cubeSide + origin.z;

    for(int j=0;j<nbDiv;j++) {

      float x,y,z,nx,ny,nz;
      float x2,y2,z2,nx2,ny2,nz2;
      float angle = (float)j/(float)nbDiv * 2.0f * PI;
      float angle2 = (float)(j+1)/(float)nbDiv * 2.0f * PI;

      // Create vertex
      switch(edges[i].orientation) {

      case DIR_OX:
        nx = 0.0;
        ny = cosf(angle);
        nz = sinf(angle);
        nx2 = 0.0;
        ny2 = cosf(angle2);
        nz2 = sinf(angle2);
        break;

      case DIR_OY:
        nx = cosf(angle);
        ny = 0.0;
        nz = sinf(angle);
        nx2 = cosf(angle2);
        ny2 = 0.0;
        nz2 = sinf(angle2);
        break;

      case DIR_OZ:
        nx = cosf(angle);
        ny = sinf(angle);
        nz = 0.0;
        nx2 = cosf(angle2);
        ny2 = sinf(angle2);
        nz2 = 0.0;
        break;

      }

      x = wEdge * nx;
      y = wEdge * ny;
      z = wEdge * nz;
      x2 = wEdge * nx2;
      y2 = wEdge * ny2;
      z2 = wEdge * nz2;

      glNormal3f(nx,ny,nz);
      glVertex3f(x1c + x,y1c + y,z1c + z);

      glNormal3f(nx,ny,nz);
      glVertex3f(x2c + x,y2c + y,z2c + z);
      
      glNormal3f(nx2,ny2,nz2);
      glVertex3f(x2c + x2,y2c + y2,z2c + z2);

      glNormal3f(nx2,ny2,nz2);
      glVertex3f(x1c + x2,y1c + y2,z1c + z2);

   }

  }

  glEnd();

  // Corner

  glBegin(GL_TRIANGLES);

  for(int i=0;i<nbCorner;i++) {

    glNormal3f(0.0f,0.0f,corner[i].o.z);
    glVertex3f(corner[i].p.x,corner[i].p.y,corner[i].p.z+wEdge*corner[i].o.z);

    glNormal3f(corner[i].o.x,0.0f,0.0f);
    glVertex3f(corner[i].p.x+wEdge*corner[i].o.x,corner[i].p.y,corner[i].p.z);

    glNormal3f(0.0f,corner[i].o.y,0.0f);
    glVertex3f(corner[i].p.x,corner[i].p.y+wEdge*corner[i].o.y,corner[i].p.z);

    //--

    glNormal3f(0.0f,0.0f,corner[i].o.z);
    glVertex3f(corner[i].p.x,corner[i].p.y,corner[i].p.z+wEdge*corner[i].o.z);

    glNormal3f(0.0f,corner[i].o.y,0.0f);
    glVertex3f(corner[i].p.x,corner[i].p.y+wEdge*corner[i].o.y,corner[i].p.z);

    glNormal3f(corner[i].o.x,0.0f,0.0f);
    glVertex3f(corner[i].p.x+wEdge*corner[i].o.x,corner[i].p.y,corner[i].p.z);

  }

  glEnd();
  glEndList();

  return GL_OK;

}


//-----------------------------------------------------------------------------

void PolyCube::gV(int cubeIdx,int x,int y,int z,float nx,float ny,float nz) {

  glNormal3f(nx,ny,nz);
  glVertex3f((cubes[cubeIdx].x+x) * cubeSide + origin.x,
             (cubes[cubeIdx].y+y) * cubeSide + origin.y,
             (cubes[cubeIdx].z+z) * cubeSide + origin.z);

}

//-----------------------------------------------------------------------------

int PolyCube::CreateGhost(int value) {

  // Create face

  ghostList = glGenLists(1);
  glNewList(ghostList,GL_COMPILE);
  glBegin(GL_QUADS);

  for(int i=0;i<nbCube;i++) {

    // Face 0
    if( IsFaceVisible(i,0) ) {
      gV(i,0,0,0, 0.0f,0.0f,-1.0f);
      gV(i,0,1,0, 0.0f,0.0f,-1.0f);
      gV(i,1,1,0, 0.0f,0.0f,-1.0f);
      gV(i,1,0,0, 0.0f,0.0f,-1.0f);
    }

    // Face 1
    if( IsFaceVisible(i,1) ) {
      gV(i,0,0,1, -1.0f,0.0f,0.0f);
      gV(i,0,1,1, -1.0f,0.0f,0.0f);
      gV(i,0,1,0, -1.0f,0.0f,0.0f);
      gV(i,0,0,0, -1.0f,0.0f,0.0f);
    }

    // Face 2
    if( IsFaceVisible(i,2) ) {
      gV(i,1,0,1, 0.0f,0.0f,1.0f);
      gV(i,1,1,1, 0.0f,0.0f,1.0f);
      gV(i,0,1,1, 0.0f,0.0f,1.0f);
      gV(i,0,0,1, 0.0f,0.0f,1.0f);
    }

    // Face 3
    if( IsFaceVisible(i,3) ) {
      gV(i,1,0,0, 1.0f,0.0f,0.0f);
      gV(i,1,1,0, 1.0f,0.0f,0.0f);
      gV(i,1,1,1, 1.0f,0.0f,0.0f);
      gV(i,1,0,1, 1.0f,0.0f,0.0f);
    }

    // Face 4
    if( IsFaceVisible(i,4) ) {
      gV(i,0,1,0, 0.0f,1.0f,0.0f);
      gV(i,0,1,1, 0.0f,1.0f,0.0f);
      gV(i,1,1,1, 0.0f,1.0f,0.0f);
      gV(i,1,1,0, 0.0f,1.0f,0.0f);
    }

    // Face 5
    if( IsFaceVisible(i,5) ) {
      gV(i,1,0,0, 0.0f,-1.0f,0.0f);
      gV(i,1,0,1, 0.0f,-1.0f,0.0f);
      gV(i,0,0,1, 0.0f,-1.0f,0.0f);
      gV(i,0,0,0, 0.0f,-1.0f,0.0f);
    }

  }

  glEnd();
  glEndList();

  // Ghost material
  float trans = ((float)value / (float)FTRANS_MAX) * 0.4f;

  ghostMaterial.Diffuse.r = 0.5f;
  ghostMaterial.Diffuse.g = 0.5f;
  ghostMaterial.Diffuse.b = 0.5f;
  ghostMaterial.Diffuse.a = trans;
  ghostMaterial.Ambient.r = 0.5f;
  ghostMaterial.Ambient.g = 0.5f;
  ghostMaterial.Ambient.b = 0.5f;
  ghostMaterial.Ambient.a = trans;

  return GL_OK;

}

//-----------------------------------------------------------------------------

void PolyCube::CopyCube(BLOCKITEM *c,int *nb) {

  for(int i=0;i<nbCube;i++) {
    c[i] = cubes[i];
  }
  *nb = nbCube;

}

//-----------------------------------------------------------------------------

VERTEX PolyCube::GetRCenter() {

  return center;

}

//-----------------------------------------------------------------------------

BLOCKITEM PolyCube::GetICenter() {

  return iCenter;

}

//-----------------------------------------------------------------------------

BOOL PolyCube::FindCube(int x,int y,int z) {

  BOOL found = FALSE;
  int i = 0;
  while(i<nbCube && !found) {
    found = (cubes[i].x == x) && (cubes[i].y == y) && (cubes[i].z == z);
    if(!found) i++;
  }

  return found;

}

//-----------------------------------------------------------------------------

BOOL PolyCube::IsFaceVisible(int cubeIdx,int face) {

  int x = cubes[cubeIdx].x;
  int y = cubes[cubeIdx].y;
  int z = cubes[cubeIdx].z;

  switch(face) {
    case 0:
      return !FindCube(x,y,z-1);
      break;
    case 1:
      return !FindCube(x-1,y,z);
      break;
    case 2:
      return !FindCube(x,y,z+1);
      break;
    case 3:
      return !FindCube(x+1,y,z);
      break;
    case 4:
      return !FindCube(x,y+1,z);
      break;
    case 5:
      return !FindCube(x,y-1,z);
      break;
  }

  return FALSE;

}

//-----------------------------------------------------------------------------

BOOL PolyCube::IsEdgeVisible(int cubeIdx,int edge) {

  int nb = 0;
  BOOL e1,e2,e3;

  int x = cubes[cubeIdx].x;
  int y = cubes[cubeIdx].y;
  int z = cubes[cubeIdx].z;

  switch(edge) {

    case 0:
      e1 = FindCube(x,y,z-1);e2 = FindCube(x,y+1,z-1);e3 = FindCube(x,y+1,z);
      break;
    case 1:
      e1 = FindCube(x,y,z-1);e2 = FindCube(x+1,y,z-1);e3 = FindCube(x+1,y,z);
      break;
    case 2:
      e1 = FindCube(x,y,z-1);e2 = FindCube(x,y-1,z-1);e3 = FindCube(x,y-1,z);
      break;
    case 3:
      e1 = FindCube(x,y,z-1);e2 = FindCube(x-1,y,z-1);e3 = FindCube(x-1,y,z);
      break;

    case 4:
      e1 = FindCube(x,y+1,z);e2 = FindCube(x-1,y+1,z);e3 = FindCube(x-1,y,z);
      break;
    case 5:
      e1 = FindCube(x,y+1,z);e2 = FindCube(x+1,y+1,z);e3 = FindCube(x+1,y,z);
      break;
    case 6:
      e1 = FindCube(x+1,y,z);e2 = FindCube(x+1,y-1,z);e3 = FindCube(x,y-1,z);
      break;
    case 7:
      e1 = FindCube(x,y-1,z);e2 = FindCube(x-1,y-1,z);e3 = FindCube(x-1,y,z);
      break;

    case 8:
      e1 = FindCube(x,y+1,z);e2 = FindCube(x,y+1,z+1);e3 = FindCube(x,y,z+1);
      break;
    case 9:
      e1 = FindCube(x+1,y,z);e2 = FindCube(x+1,y,z+1);e3 = FindCube(x,y,z+1);
      break;
    case 10:
      e1 = FindCube(x,y-1,z);e2 = FindCube(x,y-1,z+1);e3 = FindCube(x,y,z+1);
      break;
    case 11:
      e1 = FindCube(x-1,y,z);e2 = FindCube(x-1,y,z+1);e3 = FindCube(x,y,z+1);
      break;

  }

  return !( (!e1 && !e2 && e3) || (e1 && !e2 && !e3) || (e1 && e2 && e3) );

}

//-----------------------------------------------------------------------------

void PolyCube::Render(BOOL redMode) {

  glEnable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);

  if( hasGhost ) {

    // Alpha texture
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    GLApplication::SetMaterial(&ghostMaterial);

    // Draw back face first
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glCallList(ghostList);

    // Draw front face
    glCullFace(GL_BACK);
    glCallList(ghostList);

  }

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glDisable(GL_BLEND);
  glDisable(GL_CULL_FACE);

  // Draw the polycube
  if( hasBigEdge ) {

    if(redMode)
      GLApplication::SetMaterial (&redMaterial);    
    else
      GLApplication::SetMaterial (&grayMaterial);    

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glCallList(bigEdgeList);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

  } else {

    if(redMode)
      GLApplication::SetMaterial (&redMaterial);    
    else
      GLApplication::SetMaterial (&whiteMaterial);    

    glCallList(lineList);

  }

}

//-----------------------------------------------------------------------------

void PolyCube::InvalidateDeviceObjects() {

  DELETE_LIST(lineList);
  DELETE_LIST(ghostList);
  DELETE_LIST(bigEdgeList);
  nbCube = 0;
  nbOrientation = 0;

}
