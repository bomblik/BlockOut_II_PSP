/*
   File:        PageStartGame.cpp
  Description: Start Game menu page
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

void PageStartGame::Prepare(int iParam,void *pParam) {
  nbItem  = 10;
  selItem = mParent->GetSetup()->GetStartingLevel();
}

void PageStartGame::Render() {

    mParent->RenderTitle(STR("STARTING LEVEL"));
    for(int i=0;i<10;i++) {
      char tmp[32];
      sprintf(tmp,"Level %d",i);
      mParent->RenderText(14,i,(selItem==i),tmp);
    }

}

int PageStartGame::Process(BYTE *keys,float fTime) {

  int exitValue = 0;

  ProcessDefault(keys,fTime);

  if( keys[SDLK_RETURN] ) {
    mParent->GetSetup()->SetStartingLevel(selItem);
    mParent->GetSound()->PlayBlub();
    // Switch to game mode
    exitValue = 1;
    keys[SDLK_RETURN] = 0;
  }

  if( keys[SDLK_ESCAPE] ) {
     mParent->ToPage(&mParent->mainMenuPage);
     keys[SDLK_ESCAPE] = 0;
  }

  return exitValue;

}


