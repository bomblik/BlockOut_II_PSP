/*
   File:        PageChooseSetup.cpp
  Description: Choose Setup menu page
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

void PageChooseSetup::Prepare(int iParam,void *pParam) {
  nbItem  = 4;
  selItem = 0;
}

void PageChooseSetup::Render() {

  mParent->RenderTitle(STR("CHOOSE SETUP"));
  mParent->RenderText(0,0,(selItem==0),STR("Flat Fun      "));
  mParent->RenderText(0,3,(selItem==1),STR("3D Mania      "));
  mParent->RenderText(0,6,(selItem==2),STR("Out of Control"));
  mParent->RenderText(0,9,(selItem==3),STR("Change Setup  "));
  mParent->RenderText(15,0,FALSE,STR("Pit:      5x5x12"));
  mParent->RenderText(15,1,FALSE,STR("Block Set:FLAT"));
  mParent->RenderText(15,3,FALSE,STR("Pit:      3x3x10"));
  mParent->RenderText(15,4,FALSE,STR("Block Set:BASIC"));
  mParent->RenderText(15,6,FALSE,STR("Pit:      5x5x10"));
  mParent->RenderText(15,7,FALSE,STR("Block Set:EXTENDED"));

}

int PageChooseSetup::Process(BYTE *keys,float fTime) {

  ProcessDefault(keys,fTime);

  if( keys[SDLK_RETURN] ) {

    switch( selItem ) {
      case 0:  // Flat Fun
        mParent->GetSetup()->SetPitWidth(5);
        mParent->GetSetup()->SetPitHeight(5);
        mParent->GetSetup()->SetPitDepth(12);
        mParent->GetSetup()->SetBlockSet(BLOCKSET_FLAT);
        mParent->ToPage(&mParent->startGamePage);
        break;
      case 1:  // 3D Mania
        mParent->GetSetup()->SetPitWidth(3);
        mParent->GetSetup()->SetPitHeight(3);
        mParent->GetSetup()->SetPitDepth(10);
        mParent->GetSetup()->SetBlockSet(BLOCKSET_BASIC);
        mParent->ToPage(&mParent->startGamePage);
        break;
      case 2:  // Out of Control
        mParent->GetSetup()->SetPitWidth(5);
        mParent->GetSetup()->SetPitHeight(5);
        mParent->GetSetup()->SetPitDepth(10);
        mParent->GetSetup()->SetBlockSet(BLOCKSET_EXTENDED);
        mParent->ToPage(&mParent->startGamePage);
        break;
      case 3:  // Ghange setup
        mParent->ToPage(&mParent->changeSetupPage);
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

