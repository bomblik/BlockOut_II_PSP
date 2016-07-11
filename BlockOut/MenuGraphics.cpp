/*
  File:        MenuGraphics.cpp
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

#include "Menu.h"

// ---------------------------------------------------------------------

void Menu::InitGraphics() {

  startMenuTime = 0.0f;

  // Intitial rotation matrix of "BLOCKOUT" letters (Startup demo)
  
  blOrgMatrix.Init33( 1.0f , 0.0f , 0.0f ,
                      0.0f ,-1.0f , 0.0f ,
                      0.0f , 0.0f ,-1.0f );
  
  // Origin and end position of "BLOCKOUT" letters (Startup demo)

  // B
  startPos[0] = v(0.7f,-0.5f,0.0f);
  endPos[0] = v(0.65f,0.9f ,3.0f);

  // L
  startPos[1] =  v(0.25f,-0.5f,0.0f);
  endPos[1] =  v(0.25f,1.1f,3.0f);

  // O
  startPos[2] =  v(0.0f,-0.5f,0.0f);
  endPos[2] =  v(-0.15f,1.1f,3.0f);

  // C
  startPos[3] =  v(-0.25f,-0.5f,0.0f);
  endPos[3] =  v(-0.55f,1.1f,3.0f);

  // K
  startPos[4] =  v(-0.7f,-0.5f,0.0f);
  endPos[4]= v(-1.25f,0.9f,3.0f);

  // O
  startPos[5] =  v(0.25f,-0.5f,0.0f);
  endPos[5] =  v(0.25f,0.4f,3.0f);

  // U
  startPos[6] =  v(0.0f,-0.5f,0.0f);
  endPos[6] =  v(-0.15f,0.4f,3.0f);

  // T
  startPos[7] =  v( -0.25f,-0.5f,0.0f);
  endPos[7] =  v(-0.55f,0.4f,3.0f);

  // II
  startPos[8] =  v( -0.5f,-0.5f,0.0f);
  endPos[8] =  v(-1.35f,0.4f,3.0f);

  for(int i=0;i<BLLETTER_NB;i++) {
    isAdded[i] = FALSE;
    isStarted[i] = FALSE;
  }
  animEnded = FALSE;
  menuEscaped = FALSE;

}

// ---------------------------------------------------------------------

void Menu::SetViewMatrix(GLfloat *mView) {
   memcpy(matView , mView , 16*sizeof(GLfloat));
}

GLfloat *Menu::GetViewMatrix() {
   return matView;
}

// ---------------------------------------------------------------------

void Menu::FullRepaint() {
  // For the 2 back buffers
  fullRepaint = 2;
}

// ---------------------------------------------------------------------

int Menu::Create(int width,int height) {

  scrWidth = width;
  scrHeight = height;

  // Menu viewport
  menuView.x      = 0;
  menuView.y      = 0;
  menuView.width  = width;
  menuView.height = height;

  // Projection matrix
  double fAspectRatio = (double)width / (double)height;
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective(60.0,fAspectRatio,0.1,10.0);
  glGetFloatv( GL_PROJECTION_MATRIX , matProj );

  // Initialiase letters
  if( !InitBlLetters() )
    return GL_FAIL;

  // Initialiase grid
  if( !theGrid.Create() )
    return GL_FAIL;

  // Background
  int x1 = fround( (float)width  * 0.13f );
  int y1 = fround( (float)height * 0.41f );
  int x2 = fround( (float)width  * 0.87f );
  int y2 = fround( (float)height * 0.985f );

#ifndef PLATFORM_PSP
  if( !background.RestoreDeviceObjects(STR("images/menuback.png"),STR("none"),width,height) )
    return GL_FAIL;
  background.UpdateSprite(x1,y1,x2,y2,0.0f,0.0f,1.0f,1.0f);

  if(!background2.RestoreDeviceObjects(STR("images/menucredits.png"),STR("none"),width,height) )
    return GL_FAIL;
  background2.UpdateSprite(x1,y1,x2,y2,0.0f,0.0f,1.0f,1.0f);

  // Font
  wFont = fround( (float)width  * 0.0205f );
  hFont = fround( (float)height * 0.0449f );
  if( !font.RestoreDeviceObjects(STR("images/menufont.png"),STR("none"),width,height) )
    return GL_FAIL;

  if(!font2.RestoreDeviceObjects(STR("images/menufont2.png"),STR("none"),width,height))
    return GL_FAIL;

  // onLine logo
  if( !onlineLogo.RestoreDeviceObjects(STR("images/online.png"),STR("images/onlinea.png"),width,height) )
    return GL_FAIL;
#else
  if( !background.RestoreDeviceObjects(STR("images.psp/menuback.png"),STR("none"),width,height) )
    return GL_FAIL;
  background.UpdateSprite(x1,y1,x2,y2,0.0f,0.0f,1.0f,1.0f);

  if(!background2.RestoreDeviceObjects(STR("images.psp/menucredits.png"),STR("none"),width,height) )
    return GL_FAIL;
  background2.UpdateSprite(x1,y1,x2,y2,0.0f,0.0f,1.0f,1.0f);

  // Font
  wFont = fround( (float)width  * 0.0205f );
  hFont = fround( (float)height * 0.0449f );
  if( !font.RestoreDeviceObjects(STR("images.psp/menufont.png"),STR("none"),width,height) )
    return GL_FAIL;

  if(!font2.RestoreDeviceObjects(STR("images.psp/menufont2.png"),STR("none"),width,height))
    return GL_FAIL;

  // onLine logo
  if( !onlineLogo.RestoreDeviceObjects(STR("images.psp/online.png"),STR("images.psp/onlinea.png"),width,height) )
    return GL_FAIL;
#endif

  x1 = fround( 0.148f * (float)width );
  y1 = fround( 0.413f * (float)height );
  x2 = fround( 0.232f * (float)width );
  y2 = fround( 0.524f * (float)height );
  onlineLogo.UpdateSprite(x1,y1,x2,y2,0.0f,0.0f,1.0f,1.0f);

  // Controls menu page
  if( !controlsPage.Create(width,height) )
    return GL_FAIL;

  // Credits page
  if( !creditsPage.Create(width,height) )
    return GL_FAIL;

  FullRepaint();

  return GL_OK;
}

// ---------------------------------------------------------------------

void Menu::RenderChar(int x,int y,int w,int h,BOOL selected,char c) {

  BOOL ok = FALSE;
  int  fnt = 1;

  float sX = 0.0f; 
  float sY = 0.0f; 
  float eX = 0.0f; 
  float eY = 0.0f;

  if( !selected && c==' ' ) {
    // Do not render unselected space
    return;
  }

  // Space
  if( c==' ' ) {
    sX = 12.0f * 0.0638f + 0.005f; 
    sY = 0.2930f; 
    eX = 13.0f * 0.0638f + 0.003f; 
    eY = 0.3828f;
    ok = TRUE;
  }

  // Mapping
  if( c>='A' && c<='O' ) {
    sX = (float)(c-'A')   * 0.0638f + 0.005f;
    sY = 0.0f; 
    eX = (float)(c-'A'+1) * 0.0638f + 0.003f;
    eY = 0.0898f;
    ok = TRUE;
  }

  if( c>='P' && c<='Z' ) {
    sX = (float)(c-'P')   * 0.0638f + 0.005f; 
    sY = 0.0977f; 
    eX = (float)(c-'P'+1) * 0.0638f + 0.003f; 
    eY = 0.1875f;
    ok = TRUE;
  }

  if( c>='a' && c<='o' ) {
    sX = (float)(c-'a')   * 0.0638f + 0.005f;
    sY = 0.1953f; 
    eX = (float)(c-'a'+1) * 0.0638f + 0.003f;
    eY = 0.2852f;
    ok = TRUE;
  }

  if( c>='p' && c<='z' ) {
    sX = (float)(c-'p')   * 0.0638f + 0.005f; 
    sY = 0.2930f; 
    eX = (float)(c-'p'+1) * 0.0638f + 0.003f; 
    eY = 0.3828f;
    ok = TRUE;
  }

  if( c>='0' && c<='9' ) {
    sX = (float)(c-'0')   * 0.0638f + 0.005f; 
    sY = 0.3906f; 
    eX = (float)(c-'0'+1) * 0.0638f + 0.003f; 
    eY = 0.4805f;
    ok = TRUE;
  }

  if( c=='[' ) {
    sX = 10.0f   * 0.0638f + 0.005f; 
    sY = 0.3906f; 
    eX = 11.0f * 0.0638f + 0.003f; 
    eY = 0.4805f;
    ok = TRUE;
  }

  if( c==']' ) {
    sX = 11.0f   * 0.0638f + 0.005f; 
    sY = 0.3906f; 
    eX = 12.0f * 0.0638f + 0.003f; 
    eY = 0.4805f;
    ok = TRUE;
  }

  if( c==':' ) {
    sX = 12.0f   * 0.0638f + 0.005f; 
    sY = 0.3906f; 
    eX = 13.0f * 0.0638f + 0.003f; 
    eY = 0.4805f;
    ok = TRUE;
  }

  if( c=='.' ) {
    sX = 13.0f   * 0.0638f + 0.005f; 
    sY = 0.3906f; 
    eX = 14.0f * 0.0638f + 0.003f; 
    eY = 0.4805f;
    ok = TRUE;
  }

  if( c==',' ) {
    sX = 14.0f   * 0.0638f + 0.005f; 
    sY = 0.3906f; 
    eX = 15.0f * 0.0638f + 0.003f; 
    eY = 0.4805f;
    ok = TRUE;
  }

  if( c=='-' ) {
    sX = 11.0f   * 0.0638f + 0.005f; 
    sY = 0.2930f; 
    eX = 12.0f * 0.0638f + 0.003f; 
    eY = 0.3828f;
    ok = TRUE;
  }

  if( c=='+' ) {
    sX = 11.0f   * 0.0638f + 0.005f; 
    sY = 0.0977f; 
    eX = 12.0f * 0.0638f + 0.003f; 
    eY = 0.1875f;
    ok = TRUE;
  }

  if( c=='/' ) {
    sX = 12.0f   * 0.0638f + 0.005f; 
    sY = 0.0977f; 
    eX = 13.0f * 0.0638f + 0.003f; 
    eY = 0.1875f;
    ok = TRUE;
  }

  if( c=='{' ) {
    sX = 13.0f   * 0.0638f + 0.005f; 
    sY = 0.0977f; 
    eX = 14.0f * 0.0638f + 0.003f; 
    eY = 0.1875f;
    ok = TRUE;
  }

  if( c=='@' ) {
    sX = 14.0f   * 0.0638f + 0.005f; 
    sY = 0.0977f; 
    eX = 15.0f * 0.0638f + 0.003f; 
    eY = 0.1875f;
    ok = TRUE;
  }

  if( c=='\'' ) {
    sX = 14.0f   * 0.0638f + 0.005f; 
    sY = 0.2930f; 
    eX = 15.0f * 0.0638f + 0.003f; 
    eY = 0.3828f;
    ok = TRUE;
  }

  if( c=='#' ) {
    sX = 12.0f * 0.0638f + 0.005f; 
    sY = 0.2930f; 
    eX = 13.7f * 0.0638f + 0.003f; 
    eY = 0.3828f;
    ok = TRUE;
  }

  if( c=='!' ) {
    sX = 0.0f   * 0.0638f + 0.005f;
    sY = 0.0f; 
    eX = 1.0f * 0.0638f + 0.003f;
    eY = 0.0898f;
    ok = TRUE;
    fnt = 2;
  }

  if( c=='$' ) {
    sX = 1.0f   * 0.0638f + 0.005f;
    sY = 0.0f; 
    eX = 2.0f * 0.0638f + 0.003f;
    eY = 0.0898f;
    ok = TRUE;
    fnt = 2;
  }

  if( c=='%' ) {
    sX = 2.0f   * 0.0638f + 0.005f;
    sY = 0.0f; 
    eX = 3.0f * 0.0638f + 0.003f;
    eY = 0.0898f;
    ok = TRUE;
    fnt = 2;
  }

  if( c=='&' ) {
    sX = 3.0f   * 0.0638f + 0.005f;
    sY = 0.0f; 
    eX = 4.0f * 0.0638f + 0.003f;
    eY = 0.0898f;
    ok = TRUE;
    fnt = 2;
  }

  if( c=='*' ) {
    sX = 4.0f   * 0.0638f + 0.005f;
    sY = 0.0f; 
    eX = 5.0f * 0.0638f + 0.003f;
    eY = 0.0898f;
    ok = TRUE;
    fnt = 2;
  }

  if( c=='(' ) {
    sX = 5.0f   * 0.0638f + 0.005f;
    sY = 0.0f; 
    eX = 6.0f * 0.0638f + 0.003f;
    eY = 0.0898f;
    ok = TRUE;
    fnt = 2;
  }

  if( c==')' ) {
    sX = 6.0f   * 0.0638f + 0.005f;
    sY = 0.0f; 
    eX = 7.0f * 0.0638f + 0.003f;
    eY = 0.0898f;
    ok = TRUE;
    fnt = 2;
  }

  if( c=='_' ) {
    sX = 7.0f   * 0.0638f + 0.005f;
    sY = 0.0f; 
    eX = 8.0f * 0.0638f + 0.003f;
    eY = 0.0898f;
    ok = TRUE;
    fnt = 2;
  }

  if( c=='?' ) {
    sX = 8.0f   * 0.0638f + 0.005f;
    sY = 0.0f; 
    eX = 9.0f * 0.0638f + 0.003f;
    eY = 0.0898f;
    ok = TRUE;
    fnt = 2;
  }

  if( c==';' ) {
    sX = 9.0f   * 0.0638f + 0.005f;
    sY = 0.0f; 
    eX = 10.0f * 0.0638f + 0.003f;
    eY = 0.0898f;
    ok = TRUE;
    fnt = 2;
  }

  if( !ok ) return;

  if(selected) {
    sY += 0.508f; 
    eY += 0.508f;
  }

  if( fnt==1 ) {
    font.UpdateSprite(x,y,x+w,y+h,sX,sY,eX,eY);
    font.Render();
  } else {
    font2.UpdateSprite(x,y,x+w,y+h,sX,sY,eX,eY);
    font2.Render();
  }

}

// ---------------------------------------------------------------------

void Menu::RenderTitle(char *title) {

  int  lgth = (int)strlen(title);
  int  nwFont = fround((float)wFont*1.1f);
  int  nhFont = fround((float)hFont*1.1f);
  int  x1 = (scrWidth - (nwFont * lgth))/2;
  int  y1 = fround( 0.43f * (float)scrHeight );

  for(int i=0;i<lgth;i++,x1+=nwFont) {
    RenderChar(x1,y1,nwFont,nhFont,FALSE,title[i]);
  }

}

// ---------------------------------------------------------------------

void Menu::RenderText(int x,int y,BOOL selected,char *text) {

  float startLine   = 0.515f;
  float startColumn = 0.15f;
  float lineHeight  = 0.045f;

  int   lgth = (int)strlen(text);
  int   x1 = fround( startColumn*(float)scrWidth ) + x*wFont;
  int   y1 = fround( (startLine+y*lineHeight)*(float)scrHeight );

  for(int i=0;i<lgth;i++,x1+=wFont) {
    RenderChar(x1,y1,wFont,hFont,selected,text[i]);
  }

}

// ---------------------------------------------------------------------

void Menu::Render() {

  // Disable texture, alpha and z buff
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  // Set Projection and viewport
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(matProj);
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(matView);
  glViewport(menuView.x,menuView.y,menuView.width,menuView.height);

  // Clear and render the background
  if( fullRepaint>0 || !animEnded ) {

    // Menu startup demo
    glClear( GL_COLOR_BUFFER_BIT );
    theGrid.Render();

    for(int i=0;i<BLLETTER_NB;i++) {
      if( !isAdded[i] ) {
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(matView);
        glMultMatrixf(blMatrix[i].GetGL());
        blLetters[i]->Render(FALSE);
      }
    }

    fullRepaint--;

  }

  if( animEnded ) {
    if( selPage != &creditsPage )
      background.Render();
    else
      background2.Render();
    selPage->Render();
    // Restore full viewport
    glViewport(menuView.x,menuView.y,menuView.width,menuView.height);
  }

}

// ---------------------------------------------------------------------

void Menu::InvalidateDeviceObjects() {
  controlsPage.InvalidateDeviceObjects();
  creditsPage.InvalidateDeviceObjects();
  theGrid.InvalidateDeviceObjects();
  background.InvalidateDeviceObjects();
  background2.InvalidateDeviceObjects();
  font.InvalidateDeviceObjects();
  font2.InvalidateDeviceObjects();
  onlineLogo.InvalidateDeviceObjects();
  for(int i=0;i<BLLETTER_NB;i++)
    blLetters[i]->InvalidateDeviceObjects();
}

// ---------------------------------------------------------------------

void Menu::ResetAnim(float fTime) {

  startMenuTime = fTime;
  for(int i=0;i<BLLETTER_NB;i++) {
    isAdded[i] = FALSE;
    isStarted[i] = FALSE;
  }
  theGrid.Clear();
  animEnded = FALSE;

}

// ---------------------------------------------------------------------

void Menu::ProcessAnim(float fTime) {

  if( startMenuTime==0.0f ) {
    // Menu startup
    startMenuTime = fTime;
  }

  // Compute letters transformation matrix
  float angle = 0.0f;
  float xPos = 0.0f;
  float yPos = 0.0f;
  float zPos = 0.0f;
  GLMatrix t;
  GLMatrix r;
  float mTime = (fTime - startMenuTime);

  for(int i=0;i<BLLETTER_NB;i++) {

    float delay = (float)i * 0.25f; // 250ms delay between letters

    if( mTime < delay ) {
      angle =  0.0f;
      xPos  =  startPos[i].x;
      yPos  =  startPos[i].y;
      zPos  =  startPos[i].z;
    } else if( mTime > ANIMTIME+delay ) {  

      animEnded = (i==BLLETTER_NB-1);
      if( animEnded ) FullRepaint();

      // Add the letter to the grid
      if( !isAdded[i] ) {

        BLOCKITEM c[MAX_CUBE];
        int nbCube;

        isAdded[i] = TRUE;
        int ox = fround( endPos[i].x*10.0f + 0.5f ) + GRID_WIDTH/2;
        int oy = fround( endPos[i].y*10.0f ) + GRID_HEIGHT/2;
        blLetters[i]->CopyCube(c,&nbCube);
        for(int j=0;j<nbCube;j++) {
          theGrid.SetValue( ox + c[j].x , oy + c[j].y , 1);
        }
        if(!menuEscaped)
          soundManager->PlayTchh();

      }

    } else {

      if( (i==1 || i==5) && !isStarted[i] ) {
        soundManager->PlayWozz();
        isStarted[i] = TRUE;
      }

      float ratio = (mTime-delay)/ANIMTIME;
      angle = PI * ratio;
      xPos  = startPos[i].x + (endPos[i].x-startPos[i].x) * ratio;
      yPos  = startPos[i].y + (endPos[i].y-startPos[i].y) * ratio;
      zPos  = startPos[i].z + (endPos[i].z-startPos[i].z) * ratio;

    }

    t.Translate(xPos,yPos,zPos);
    r.RotateX(angle);
    blMatrix[i].Identity();
    blMatrix[i].Multiply(&t);
    blMatrix[i].Multiply(&r);
    blMatrix[i].Multiply(&blOrgMatrix);

  }


}

// ---------------------------------------------------------------------

int Menu::InitBlLetters() {

  int hr;

  VERTEX org = v(0.0f,0.0f,0.0f);

  // "B" letter
  blLetters[0] = new PolyCube();
  blLetters[0]->AddCube(0,0,0);
  blLetters[0]->AddCube(1,0,0);
  blLetters[0]->AddCube(2,0,0);
  blLetters[0]->AddCube(3,0,0);
  blLetters[0]->AddCube(4,0,0);
  blLetters[0]->AddCube(5,0,0);
  blLetters[0]->AddCube(0,1,0);
  blLetters[0]->AddCube(4,1,0);
  blLetters[0]->AddCube(0,2,0);
  blLetters[0]->AddCube(4,2,0);
  blLetters[0]->AddCube(0,3,0);
  blLetters[0]->AddCube(1,3,0);
  blLetters[0]->AddCube(2,3,0);
  blLetters[0]->AddCube(3,3,0);
  blLetters[0]->AddCube(4,3,0);
  blLetters[0]->AddCube(1,4,0);
  blLetters[0]->AddCube(4,4,0);
  blLetters[0]->AddCube(1,5,0);
  blLetters[0]->AddCube(4,5,0);
  blLetters[0]->AddCube(1,6,0);
  blLetters[0]->AddCube(2,6,0);
  blLetters[0]->AddCube(3,6,0);
  blLetters[0]->AddCube(4,6,0);
  blLetters[0]->AddCube(5,6,0);
  hr = blLetters[0]->Create(0.1f,org,FALSE);
  if(!hr) return GL_FAIL;

  // "L" letter
  blLetters[1] = new PolyCube();
  blLetters[1]->AddCube(0,0,0);
  blLetters[1]->AddCube(1,0,0);
  blLetters[1]->AddCube(2,0,0);
  blLetters[1]->AddCube(2,1,0);
  blLetters[1]->AddCube(2,2,0);
  blLetters[1]->AddCube(2,3,0);
  blLetters[1]->AddCube(2,4,0);
  hr = blLetters[1]->Create(0.1f,org,FALSE);
  if(!hr) return GL_FAIL;

  // "O" letter
  blLetters[2] = new PolyCube();
  blLetters[2]->AddCube(0,0,0);
  blLetters[2]->AddCube(1,0,0);
  blLetters[2]->AddCube(2,0,0);
  blLetters[2]->AddCube(0,1,0);
  blLetters[2]->AddCube(2,1,0);
  blLetters[2]->AddCube(0,2,0);
  blLetters[2]->AddCube(2,2,0);
  blLetters[2]->AddCube(0,3,0);
  blLetters[2]->AddCube(2,3,0);
  blLetters[2]->AddCube(0,4,0);
  blLetters[2]->AddCube(1,4,0);
  blLetters[2]->AddCube(2,4,0);
  hr = blLetters[2]->Create(0.1f,org,FALSE);
  if(!hr) return GL_FAIL;

  // "C" letter
  blLetters[3] = new PolyCube();
  blLetters[3]->AddCube(0,0,0);
  blLetters[3]->AddCube(1,0,0);
  blLetters[3]->AddCube(2,0,0);
  blLetters[3]->AddCube(0,1,0);
  blLetters[3]->AddCube(2,1,0);
  blLetters[3]->AddCube(2,2,0);
  blLetters[3]->AddCube(0,3,0);
  blLetters[3]->AddCube(2,3,0);
  blLetters[3]->AddCube(0,4,0);
  blLetters[3]->AddCube(1,4,0);
  blLetters[3]->AddCube(2,4,0);
  hr = blLetters[3]->Create(0.1f,org,FALSE);
  if(!hr) return GL_FAIL;


  // "K" letter
  blLetters[4] = new PolyCube();
  blLetters[4]->AddCube(0,0,0);
  blLetters[4]->AddCube(1,0,0);
  blLetters[4]->AddCube(4,0,0);
  blLetters[4]->AddCube(5,0,0);
  blLetters[4]->AddCube(1,1,0);
  blLetters[4]->AddCube(4,1,0);
  blLetters[4]->AddCube(2,2,0);
  blLetters[4]->AddCube(4,2,0);
  blLetters[4]->AddCube(3,3,0);
  blLetters[4]->AddCube(4,3,0);
  blLetters[4]->AddCube(2,4,0);
  blLetters[4]->AddCube(4,4,0);
  blLetters[4]->AddCube(1,5,0);
  blLetters[4]->AddCube(4,5,0);
  blLetters[4]->AddCube(0,6,0);
  blLetters[4]->AddCube(1,6,0);
  blLetters[4]->AddCube(4,6,0);
  blLetters[4]->AddCube(5,6,0);
  hr = blLetters[4]->Create(0.1f,org,FALSE);
  if(!hr) return GL_FAIL;

  // "O" letter
  blLetters[5] = new PolyCube();
  blLetters[5]->AddCube(0,0,0);
  blLetters[5]->AddCube(1,0,0);
  blLetters[5]->AddCube(2,0,0);
  blLetters[5]->AddCube(0,1,0);
  blLetters[5]->AddCube(2,1,0);
  blLetters[5]->AddCube(0,2,0);
  blLetters[5]->AddCube(2,2,0);
  blLetters[5]->AddCube(0,3,0);
  blLetters[5]->AddCube(2,3,0);
  blLetters[5]->AddCube(0,4,0);
  blLetters[5]->AddCube(1,4,0);
  blLetters[5]->AddCube(2,4,0);
  hr = blLetters[5]->Create(0.1f,org,FALSE);
  if(!hr) return GL_FAIL;

  // "U" letter
  blLetters[6] = new PolyCube();
  blLetters[6]->AddCube(0,0,0);
  blLetters[6]->AddCube(1,0,0);
  blLetters[6]->AddCube(2,0,0);
  blLetters[6]->AddCube(0,1,0);
  blLetters[6]->AddCube(2,1,0);
  blLetters[6]->AddCube(0,2,0);
  blLetters[6]->AddCube(2,2,0);
  blLetters[6]->AddCube(0,3,0);
  blLetters[6]->AddCube(2,3,0);
  blLetters[6]->AddCube(0,4,0);
  blLetters[6]->AddCube(2,4,0);
  hr = blLetters[6]->Create(0.1f,org,FALSE);
  if(!hr) return GL_FAIL;

  // "T" letter
  blLetters[7] = new PolyCube();
  blLetters[7]->AddCube(1,0,0);
  blLetters[7]->AddCube(1,1,0);
  blLetters[7]->AddCube(1,2,0);
  blLetters[7]->AddCube(1,3,0);
  blLetters[7]->AddCube(0,4,0);
  blLetters[7]->AddCube(1,4,0);
  blLetters[7]->AddCube(2,4,0);
  hr = blLetters[7]->Create(0.1f,org,FALSE);
  if(!hr) return GL_FAIL;

  // "II" letter
  blLetters[8] = new PolyCube();
  blLetters[8]->AddCube(0,0,0);
  blLetters[8]->AddCube(1,0,0);
  blLetters[8]->AddCube(2,0,0);
  blLetters[8]->AddCube(3,0,0);
  blLetters[8]->AddCube(4,0,0);
  blLetters[8]->AddCube(1,1,0);
  blLetters[8]->AddCube(1,2,0);
  blLetters[8]->AddCube(3,1,0);
  blLetters[8]->AddCube(3,2,0);
  blLetters[8]->AddCube(0,3,0);
  blLetters[8]->AddCube(1,3,0);
  blLetters[8]->AddCube(2,3,0);
  blLetters[8]->AddCube(3,3,0);
  blLetters[8]->AddCube(4,3,0);
  hr = blLetters[8]->Create(0.1f,org,FALSE);
  if(!hr) return GL_FAIL;

  return GL_OK;
}
