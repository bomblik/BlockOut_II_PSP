/*
   File:        Menu.h
  Description: Contains menu pages.
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

#ifndef MENUPAGEH
#define MENUPAGEH

#include <math.h>
#include "GLApp/GLSprite.h"
#include "GLApp/GLMatrix.h"
#include "PolyCube.h"

#ifdef PLATFORM_PSP
#include "platforms/PlatformPSP.h"
#endif

class Menu;

#include "EditControl.h"

#define DECLARE(className,exts) class className: public MenuPage {                \
                                public:                                           \
                                  className() {};                                 \
                                  void Prepare(int iParam,void *pParam);          \
                                  void Render();                                  \
                                  int  Process(BYTE *keys,float fTime);           \
                                private:                                          \
                                  exts                                            \
                                }

// Super Class
class MenuPage {
public:

  // Init parent
  void SetParent(Menu *parent) {mParent=parent;};

  // Overrides
  virtual void Prepare(int iParam,void *pParam) {};
  virtual void Render() {};
  virtual int  Process(BYTE *keys,float fTime) { return 0; };
  
 protected:

   void ProcessDefault(BYTE *keys,float fTime) {

    if(keys[SDLK_DOWN]) {
      selItem++;
      if(selItem>=nbItem) selItem=0;
      keys[SDLK_DOWN] = 0;
    }
     
    if(keys[SDLK_UP]) {
      selItem--;
      if(selItem<0) selItem=nbItem-1;
      keys[SDLK_UP] = 0;
    }

  }

  int selItem;
  int nbItem;
  Menu *mParent;

};

// Sub classes
DECLARE(PageMainMenu,
  float startWriteTime;
);

DECLARE(PageStartGame,);
DECLARE(PageChooseSetup,);

DECLARE(PageChangeSetup,
  void ProcessKey(int keyCode);
);

DECLARE(PageHallOfFame,
  SCOREREC   allScore[10];
  BOOL       editMode;
  char       editText[11];
  int        editPos;
  BOOL       editCursor;
  SCOREREC  *editScore;
  float      startEditTime;
  PLAYER_INFO pInfo;

  void ProcessEdit(BYTE *keys,float fTime);
public:
  PLAYER_INFO *GetPlayer();
);

DECLARE(PageOptions,);

DECLARE(PageGSOptions,
  int ProcessKey(int keyCode);
);

DECLARE(PageScoreDetails,
  SCOREREC  *score;
  int rank;
  int   pageState;
  float startTime;
  char  errMsg[512];
);

DECLARE(PageControls,
    GLVIEWPORT   blockView;
    GLfloat      blockProj[16];
    Sprite2D     pitBack;
    GLfloat      matBlock[3][16];
    PolyCube   block[3];
    float      startTime;
    float      cubeSide;
    int        rotPos;
    BOOL       rotMode;
    BOOL       editMode;
    float      startEditTime;
    BOOL       cursorVisible;

    void ProcessMenuDis(BYTE *keys);
    void ProcessEdit(BYTE *keys);
    void ProcessAnimation(float fTime);
    void Place(PolyCube *obj,GLfloat *mat,GLMatrix *matR,float x,float y,float z);
    void RenderKey(int x,int y,char k,int pos);
  public:
    int Create(int width,int height);
    void InvalidateDeviceObjects();
);

DECLARE(PageHttp,
  EditControl edit;
  char *Wrap(char *str);
);

DECLARE(PageHallOfFameOnLine,
  SCOREREC   allScore[100];
  int        startPos;
  int        downloadState;
  char       errMsg[512];
  char      *pPos;
  PLAYER_INFO pInfo;

#ifdef PLATFORM_PSP
  PlatformPSP psp;
#endif

  void  FetchNextLine();
  char *FetchNextField();
  void  ParseScore(SCOREREC *s);
  void  DownloadScore();
public:
  PLAYER_INFO *GetPlayer();
);

DECLARE(PageCredits,
   GLVIEWPORT   blockView;
   GLfloat      blockProj[16];
   GLfloat      matBlock[16];
   PolyCube     block;
   float        startTime;
  public:
   int Create(int width,int height);
   void InvalidateDeviceObjects();
);

#endif /* MENUPAGEH */
