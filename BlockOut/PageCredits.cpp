/*
   File:        PageCredits.cpp
  Description: Credits page
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

#include "Menu.h"
// -----------------------------------------------------------

void PageCredits::Prepare(int iParam,void *pParam) {
  nbItem  = 0;
  selItem = 0;
  startTime = 0.0f;
  mParent->GetSound()->PlayMusic();
}

// -----------------------------------------------------------

void PageCredits::Render() {

  mParent->RenderTitle(STR("CREDITS"));

  // Render demo block
  glViewport(blockView.x,blockView.y,blockView.width,blockView.height);
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(blockProj);
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(mParent->GetViewMatrix());
  glMultMatrixf(matBlock);
  block.Render(FALSE);

}

// -----------------------------------------------------------

int PageCredits::Process(BYTE *keys,float fTime) {

  if( keys[SDLK_ESCAPE] ) {
    mParent->GetSound()->StopMusic();
    mParent->ToPage(&mParent->mainMenuPage);
    keys[SDLK_ESCAPE] = 0;
  }

  if(startTime==0.0f)
    startTime = fTime;

  float aTime = fTime - startTime;

  GLMatrix t1,t2,rx,ry,rz,res;

  t1.Translate(0.0f,0.0f,-0.45f);
  rx.RotateX(aTime * PI/2.0f);
  ry.RotateY(aTime * PI/2.5f);
  rz.RotateZ(aTime * PI/3.0f);
  t2.Translate(0.0f,0.0f,0.45f);
  res.Multiply(&t2);
  res.Multiply(&rz);
  res.Multiply(&ry);
  res.Multiply(&rx);
  res.Multiply(&t1);
  memcpy( matBlock , res.GetGL() , 16 * sizeof(GLfloat) );

  return 0;
}

// -----------------------------------------------------------

int PageCredits::Create(int width,int height) {

  VERTEX org = v(-0.15f,-0.05f,0.4f);

  block.AddCube(1,0,-1);
  block.AddCube(1,1,-1);
  block.AddCube(1,-1,0);
  block.AddCube(1,0,0);
  block.AddCube(0,0,0);
  if(!block.Create(0.1f,org,8)) return GL_FAIL;

  // block viewport 
  float X = (float)width  * 0.216f;
  float Y = (float)height * 0.409f;
  float W = (float)width  * 0.572f;
  float H = (float)height * 0.572f;
  
  blockView.x      = fround(X);
  blockView.y      = height - (fround(Y+H));
  blockView.width  = fround(W);
  blockView.height = fround(H);

  // Projection matrix
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  float ratio = W/H;
  gluPerspective(60.0,ratio,0.1,FAR_DISTANCE);
  glGetFloatv( GL_PROJECTION_MATRIX , blockProj );

  return GL_OK;

}

// -----------------------------------------------------------

void PageCredits::InvalidateDeviceObjects() {

  block.InvalidateDeviceObjects();

}


