/*
   File:        PageOptions.cpp
  Description: Options menu page
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

void PageOptions::Prepare(int iParam,void *pParam) {
  nbItem  = 3;
  selItem = 0;
}

void PageOptions::Render() {
  
  mParent->RenderTitle(STR("OPTIONS"));
  mParent->RenderText(0,0,(selItem==0),STR("Controls          "));
  mParent->RenderText(0,1,(selItem==1),STR("Graphics & Sound  "));
  mParent->RenderText(0,2,(selItem==2),STR("HTTP              "));

}

int PageOptions::Process(BYTE *keys,float fTime) {

  ProcessDefault(keys,fTime);

  if( keys[SDLK_RETURN] ) {
    switch( selItem ) {
      case 0: // Controls
        mParent->ToPage(&mParent->controlsPage);
        break;
      case 1: // Graphics and Sound
        mParent->ToPage(&mParent->gsOptionsPage);
        break;
      case 2: // HTTP
        mParent->ToPage(&mParent->httpPage);
        break;
    }
    keys[SDLK_RETURN] = 0;
  }

  if( keys[SDLK_ESCAPE] ) {
     mParent->ToPage(&mParent->mainMenuPage);
     keys[SDLK_ESCAPE] = 0;
  }

  return 0;
}
