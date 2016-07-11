/*
   File:        PageMainMenu.cpp
  Description: Main menu page
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

void PageMainMenu::Prepare(int iParam,void *pParam) {
  nbItem  = 10;
  selItem = 0;
  startWriteTime = 0.0f;
}

void PageMainMenu::Render() {

  mParent->RenderTitle(STR("MAIN MENU"));
  mParent->RenderText(0,0,(selItem==0),STR("Start Game  "));
  mParent->RenderText(0,1,(selItem==1),STR("Choose Setup"));
  mParent->RenderText(0,2,(selItem==2),STR("Hall of Fame"));
  mParent->RenderText(0,3,(selItem==3),STR("Online Score"));
  mParent->RenderText(0,4,(selItem==4),STR("Options     "));
  mParent->RenderText(0,5,(selItem==5),STR("Write Setup "));
  if( startWriteTime!=0.0f ) mParent->RenderText(12,5,FALSE,STR("[Done]"));
  mParent->RenderText(0,6,(selItem==6),STR("Demo        "));
  mParent->RenderText(0,7,(selItem==7),STR("Practice    "));
  mParent->RenderText(0,8,(selItem==8),STR("Credits     "));
  mParent->RenderText(0,9,(selItem==9),STR("Quit        "));
  mParent->RenderText(13,0,FALSE,mParent->GetSetup()->GetName());

}

int PageMainMenu::Process(BYTE *keys,float fTime) {

  int exitValue = 0;

  if( startWriteTime!=0.0f ) {
    if( fTime-startWriteTime>0.5f ) {
      startWriteTime = 0.0f;
    } else {
      return exitValue;
    }
  }

  ProcessDefault(keys,fTime);

  if( keys[SDLK_RETURN] ) {
    switch(selItem) {
      case 0: // Start game
        mParent->ToPage(&mParent->startGamePage);
        break;
      case 1: // Choose Setup
       mParent->ToPage(&mParent->chooseSetupPage);
       break;
      case 2: // Hall of Fame
        mParent->ToPage(&mParent->hallOfFamePage);
        break;
      case 3: // Hall of Fame [On Line]
        mParent->ToPage(&mParent->hallOfFamePageOnLine);
        break;
      case 4: // Options
        mParent->ToPage(&mParent->optionsPage);
        break;
      case 5: // Write setup
        mParent->GetSetup()->WriteSetup();
        startWriteTime = fTime;
        break;
      case 6: // Demo mode
        exitValue = 7;
        break;
      case 7: // Practice mode
        exitValue = 8;
        break;
      case 8: // Credits
        mParent->ToPage(&mParent->creditsPage);
        break;
      case 9: // Quit
        exitValue = 100;
        break;
    }
    keys[SDLK_RETURN] = 0;
  }

  if( keys[SDLK_ESCAPE] ) {
    exitValue = 100;
    keys[SDLK_ESCAPE] = 0;
  }

  return exitValue;
}

