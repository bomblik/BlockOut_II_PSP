/*
   File:        PageControls.cpp
  Description: Controls menu page
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

void PageControls::Prepare(int iParam,void *pParam) {
  nbItem  = 8;
  selItem = 2;
  startTime = 0.0f;
  rotPos = 0;
  editMode = FALSE;
  cursorVisible = FALSE;
}

// -----------------------------------------------------------

void PageControls::Render() {

  pitBack.Render();

#ifndef PLATFORM_PSP
  mParent->RenderTitle(STR("GAME CONTROLS"));

  RenderKey(4,5,mParent->GetSetup()->GetKRx1(),2);
  RenderKey(4,6,mParent->GetSetup()->GetKRx2(),3);
  RenderKey(16,5,mParent->GetSetup()->GetKRy1(),4);
  RenderKey(16,6,mParent->GetSetup()->GetKRy2(),5);
  RenderKey(27,5,mParent->GetSetup()->GetKRz1(),6);
  RenderKey(27,6,mParent->GetSetup()->GetKRz2(),7);

  if( rotMode ) {
    mParent->RenderText(2,6,FALSE,STR("-"));
    mParent->RenderText(31,6,FALSE,STR("-"));
  } else {
    mParent->RenderText(2,5,FALSE,STR("-"));
    mParent->RenderText(31,5,FALSE,STR("-"));
  }

  mParent->RenderText(0,7,FALSE,STR("Auto:"));
  mParent->RenderText(9,7,(selItem==0),STR("[QWERTY]"));
  mParent->RenderText(18,7,(selItem==1),STR("[AZERTY]"));

  if( !editMode )
    mParent->RenderText(0,9,FALSE,STR("[Cursor]Move [Space]Drop [P]Pause"));
  else
    mParent->RenderText(0,9,FALSE,STR("Press new key or [Esc] to cancel"));
#else
  mParent->GetSetup()->ResetToQwerty();

  mParent->RenderTitle(STR("GAME CONTROLS"));

  RenderKey(3,5,mParent->GetSetup()->GetKRx1(),2);
  RenderKey(3,6,mParent->GetSetup()->GetKRx2(),3);
  RenderKey(12,5,mParent->GetSetup()->GetKRy1(),4);
  RenderKey(12,6,mParent->GetSetup()->GetKRy2(),5);
  RenderKey(22,5,mParent->GetSetup()->GetKRz1(),6);
  RenderKey(22,6,mParent->GetSetup()->GetKRz2(),7);

  if( rotMode ) {
    mParent->RenderText(2,6,TRUE,STR("-"));
    mParent->RenderText(31,6,TRUE,STR("-"));
  } else {
    mParent->RenderText(2,5,TRUE,STR("-"));
    mParent->RenderText(31,5,TRUE,STR("-"));
  }

    mParent->RenderText(0,8,FALSE,STR("[DPAD] :Move    [CROSS]   :Drop"));
    mParent->RenderText(0,9,FALSE,STR("[START]:Pause   [RTRIGGER]:Cancel"));
#endif

  // Render sample blocks
  glViewport(blockView.x,blockView.y,blockView.width,blockView.height);
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(blockProj);
  for(int i=0;i<3;i++) {
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(mParent->GetViewMatrix());
    glMultMatrixf(matBlock[i]);
    BOOL redMode = editMode && ((!rotMode && (selItem==2*i+2)) || (rotMode && (selItem==2*i+3)));
    block[i].Render(redMode);
  }

}

// -----------------------------------------------------------

void PageControls::RenderKey(int x,int y,char k,int pos) {

  char tmp[256];

#ifndef PLATFORM_PSP
  if( !editMode || selItem!=pos ) {
    sprintf(tmp,"[%c]",k);
    mParent->RenderText(x,y,(selItem==pos),tmp);
  } else {
    mParent->RenderText(x,y,FALSE,STR("["));
    mParent->RenderText(x+1,y,cursorVisible,STR(" "));
    mParent->RenderText(x+2,y,FALSE,STR("]"));
  }
#else
  if( !editMode || selItem!=pos ) {
    if (k == 'Q')
      sprintf(tmp,"[%s]","SQUARE");
    else if (k == 'W')
      sprintf(tmp,"[%s]","TRIANGLE");
    else if (k == 'E')
      sprintf(tmp,"[%s]","CIRCLE");
    else if (k == 'A')
    {
      sprintf(tmp,"[%s","LTRIGGER");
      mParent->RenderText(x,y,FALSE,tmp);
      sprintf(tmp,"%s]","+SQUARE");
      mParent->RenderText(x,y + 1,FALSE,tmp);
      return;
    }
    else if (k == 'S')
    {
      sprintf(tmp,"[%s","LTRIGGER");
      mParent->RenderText(x,y,FALSE,tmp);
      sprintf(tmp,"%s]","+TRIANGLE");
      mParent->RenderText(x,y + 1,FALSE,tmp);
      return;
    }
    else if (k == 'D')
    {
      sprintf(tmp,"[%s","LTRIGGER");
      mParent->RenderText(x,y,FALSE,tmp);
      sprintf(tmp,"%s]","+CIRCLE");
      mParent->RenderText(x,y + 1,FALSE,tmp);
      return;
    }

    mParent->RenderText(x,y,FALSE,tmp);
  } else {
    mParent->RenderText(x,y,FALSE,STR("["));
    mParent->RenderText(x+1,y,FALSE,STR(" "));
    mParent->RenderText(x+2,y,FALSE,STR("]"));
  }
#endif

}

// -----------------------------------------------------------

void PageControls::ProcessAnimation(float fTime) {

  // Process block animation
  if( startTime == 0.0f )
    startTime = fTime;

  GLMatrix matR;

  float aTime = fTime-startTime;
  float startAngle = (float)rotPos * PI/2.0f;
  float angle = startAngle + PI*aTime;

  if( aTime >= 0.5f && aTime <= 0.75 ) {

    // Make a short pause
    angle = startAngle + PI/2.0f;

  } else if( aTime > 0.75f ) {

    startTime = fTime;
    rotPos++;
    if( rotPos >= 4 )
      rotPos = 0;
    angle = startAngle + PI/2.0f;

  }

  if( rotMode ) {

    //ASD
    matR.RotateX(angle);
    Place(block + 0,&(matBlock[0][0]),&matR,6.0f, 1.0f,1.0f);

    matR.RotateY(-angle);
    Place(block + 1,&(matBlock[1][0]),&matR,2.0f, 0.0f,1.0f);

    matR.RotateZ(angle);
    Place(block + 2,&(matBlock[2][0]),&matR,0.0f, 1.0f,1.0f);

  } else {

    // QWE
    matR.RotateX(-angle);
    Place(block + 0,&(matBlock[0][0]),&matR,6.0f, 1.0f,1.0f);

    matR.RotateY(angle);
    Place(block + 1,&(matBlock[1][0]),&matR,2.0f, 0.0f,1.0f);

    matR.RotateZ(-angle);
    Place(block + 2,&(matBlock[2][0]),&matR,0.0f, 1.0f,1.0f);

  }


}

// -----------------------------------------------------------

void PageControls::ProcessEdit(BYTE *keys) {

  if( keys[SDLK_ESCAPE] ) {
    editMode = false;
    keys[SDLK_ESCAPE] = 0;
  }

  // Search key
  BOOL found = FALSE;
  BYTE i  = 'A';
  BYTE i2 = 'a';
  while( i<='Z' && !found ) {
    found = ( keys[i]!=0 || keys[i2]!=0 );
    if(!found) { i++;i2++; }
  }

  if( found ) {
    switch(selItem) {
      case 2: // Q
        mParent->GetSetup()->SetKRx1(i);
      break;
      case 4: // W
        mParent->GetSetup()->SetKRy1(i);
      break;
      case 6: // E
        mParent->GetSetup()->SetKRz1(i);
      break;
      case 3: // A
        mParent->GetSetup()->SetKRx2(i);
      break;
      case 5: // Z
        mParent->GetSetup()->SetKRy2(i);
      break;
      case 7:  // D
        mParent->GetSetup()->SetKRz2(i);
      break;
    }
    editMode = FALSE;
    keys[i] = 0;
  }

}

// -----------------------------------------------------------

void PageControls::ProcessMenuDis(BYTE *keys) {

  int dir = 0;
  if( (dir==0) && keys[SDLK_LEFT] ) dir = 1;
  if( (dir==0) && keys[SDLK_RIGHT]) dir = 2;
  if( (dir==0) && keys[SDLK_UP]   ) dir = 3;
  if( (dir==0) && keys[SDLK_DOWN] ) dir = 4;

  // Menu displacement
  switch(selItem) {
    case 0:
      switch(dir) {
        case 2:
          selItem = 1;
          break;
        case 3:
          selItem = 3;
          break;
      }
    break;
    case 1:
      switch(dir) {
        case 1:
          selItem = 0;
          break;
        case 3:
          selItem = 7;
          break;
      }
    break;
    case 2:
      switch(dir) {
        case 2:
          selItem = 4;
          break;
        case 4:
          selItem = 3;
          break;
      }
    break;
    case 3:
      switch(dir) {
        case 2:
          selItem = 5;
          break;
        case 3:
          selItem = 2;
          break;
        case 4:
          selItem = 0;
          break;
      }
    break;
    case 4:
      switch(dir) {
        case 1:
          selItem = 2;
          break;
        case 2:
          selItem = 6;
          break;
        case 4:
          selItem = 5;
          break;
      }
    break;
    case 5:
      switch(dir) {
        case 1:
          selItem = 3;
          break;
        case 2:
          selItem = 7;
          break;
        case 3:
          selItem = 4;
          break;
        case 4:
          selItem = 0;
          break;
      }
    break;
    case 6:
      switch(dir) {
        case 1:
          selItem = 4;
          break;
        case 4:
          selItem = 7;
          break;
      }
    break;
    case 7:
      switch(dir) {
        case 1:
          selItem = 5;
          break;
        case 3:
          selItem = 6;
          break;
        case 4:
          selItem = 1;
          break;
      }
    break;
  }

}

// -----------------------------------------------------------

int PageControls::Process(BYTE *keys,float fTime) {

  if( !editMode )
    ProcessMenuDis(keys);
  else
    ProcessEdit(keys);

  // Reset cursor key
  keys[SDLK_LEFT] = 0;
  keys[SDLK_RIGHT] = 0;
  keys[SDLK_UP] = 0;
  keys[SDLK_DOWN] = 0;

  if( keys[SDLK_RETURN] ) {
    switch(selItem) {
      case 0: // Reset to Qwerty
        mParent->GetSetup()->ResetToQwerty();
        break;
      case 1: // Reset to Azerty
        mParent->GetSetup()->ResetToAzerty();
        break;
      case 2: // Configure Q
      case 4: // Configure W
      case 6: // Configure E
      case 3: // Configure A
      case 5: // Configure S
      case 7: // Configure D
        for(int i='A';i<='Z';i++) keys[i] = 0;
        for(int i='a';i<='z';i++) keys[i] = 0;
#ifndef PLATFORM_PSP
        editMode = TRUE;
#endif
        startEditTime = fTime;
        break;
    }
    keys[SDLK_RETURN] = 0;
  }

  if( keys[SDLK_ESCAPE] ) {
    mParent->ToPage(&mParent->optionsPage);
    keys[SDLK_ESCAPE] = 0;
  }

  // rotation mode
  rotMode = (selItem==3) ||  (selItem==5) || (selItem==7);

  // Cursor
  if( editMode )
    cursorVisible = ( (fround((startEditTime - fTime) * 2.0f)%2) == 0 );
  else
    cursorVisible = FALSE;

  ProcessAnimation(fTime);

  return 0;
}

// -----------------------------------------------------------

void PageControls::Place(PolyCube *obj,GLfloat *mat,GLMatrix *matR,float x,float y,float z) {

  GLMatrix matT;
  GLMatrix matT1;
  GLMatrix matT2;
  GLMatrix mRes;

  VERTEX center = obj->GetRCenter();
  matT1.Translate(-center.x,-center.y,-center.z);
  matT2.Translate(center.x,center.y,center.z);
  x = (x * cubeSide);
  y = (y * cubeSide);
  z = (z * cubeSide);
  matT.Translate(x,y,z);

  mRes.Multiply(&matT);
  mRes.Multiply(&matT2);
  mRes.Multiply(matR);
  mRes.Multiply(&matT1);
  memcpy( mat , mRes.GetGL() , 16 * sizeof(GLfloat) );

}

// -----------------------------------------------------------

int PageControls::Create(int width,int height) {

  // Create pit background
  float pitX = (float)width  * 0.2187f;
  float pitY = (float)height * 0.485f;
  float pitW = (float)width  * 0.5625f;
  float pitH = (float)height * 0.25f;

#ifndef PLATFORM_PSP
  if( !pitBack.RestoreDeviceObjects(STR("images/menupit.png"),STR("none"),width,height) )
#else
  if( !pitBack.RestoreDeviceObjects(STR("images.psp/menupit.png"),STR("none"),width,height) )
#endif
    return GL_FAIL;
  pitBack.UpdateSprite(fround(pitX),fround(pitY),
                       fround(pitX+pitW),fround(pitY+pitH),
                       0.0f,0.0f,1.0f,0.337f);

  // Emulate pit 9*3*12
  cubeSide = 1.0f/9.0f;
  VERTEX org = v(-0.5f,-1.5f*cubeSide,STARTZ);

  block[0].AddCube(0,0,0);
  block[0].AddCube(1,0,0);
  block[0].AddCube(2,0,0);
  block[0].AddCube(2,1,0);
  if( !block[0].Create(cubeSide,org,FALSE) ) return GL_FAIL;

  block[1].AddCube(1,0,0);
  block[1].AddCube(2,0,0);
  block[1].AddCube(1,2,0);
  block[1].AddCube(1,1,0);
  if(!block[1].Create(cubeSide,org,FALSE) ) return GL_FAIL;

  block[2].AddCube(0,0,0);
  block[2].AddCube(1,0,0);
  block[2].AddCube(1,1,0);
  if(!block[2].Create(cubeSide,org,FALSE)) return GL_FAIL;

  // block viewport 
  // Build a square viewport, pit background image has an aspect ratio of 3
  int vY = fround( pitY - pitH );
  int vH = fround( 3.0f * pitH );
  
  if( vH+vY > (int)height ) {
    // Should not happen
    vH = height - vY;
  }

  blockView.x      = fround(pitX);
  blockView.y      = height - (vY + vH);
  blockView.width  = fround(pitW);
  blockView.height = vH;

  // Projection matrix
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective(60.0,1.0,0.1,FAR_DISTANCE);
  glGetFloatv( GL_PROJECTION_MATRIX , blockProj );

  return GL_OK;
}

// -----------------------------------------------------------

void PageControls::InvalidateDeviceObjects() {

  pitBack.InvalidateDeviceObjects();
  for(int i=0;i<3;i++)
    block[i].InvalidateDeviceObjects();

}

