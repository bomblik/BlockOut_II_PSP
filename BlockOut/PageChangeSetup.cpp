/*
   File:        PageChangeSetup.cpp
  Description: Change Setup menu page
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

void PageChangeSetup::Prepare(int iParam,void *pParam) {
  nbItem  = 5;
  selItem = 0;
}

void PageChangeSetup::Render() {

   char tmp[256];

   mParent->RenderTitle(STR("CHANGE SETUP"));
   mParent->RenderText(0,0,(selItem==0),STR("Pit Length :"));
   mParent->RenderText(0,1,(selItem==1),STR("Pit Width  :"));
   mParent->RenderText(0,2,(selItem==2),STR("Pit Depth  :"));
   mParent->RenderText(0,3,(selItem==3),STR("Block Set  :"));
   mParent->RenderText(0,4,(selItem==4),STR("Start Game  "));

   sprintf(tmp,"%d",mParent->GetSetup()->GetPitWidth());
   mParent->RenderText(13,0,FALSE,tmp);
   sprintf(tmp,"%d",mParent->GetSetup()->GetPitHeight());
   mParent->RenderText(13,1,FALSE,tmp);
   sprintf(tmp,"%d",mParent->GetSetup()->GetPitDepth());
   mParent->RenderText(13,2,FALSE,tmp);
   mParent->RenderText(13,3,FALSE,(char *)mParent->GetSetup()->GetBlockSetName());

}

int PageChangeSetup::Process(BYTE *keys,float fTime) {

  ProcessDefault(keys,fTime);

  if( keys[SDLK_RETURN] ) {
    switch( selItem ) {
      case 0:  // Pit width
      case 1:  // Pit height
      case 2:  // Pit Depth
      case 3:  // Block set
        ProcessKey(SDLK_RIGHT);
        break;
      case 4:  // Start game
        mParent->ToPage(&mParent->startGamePage);
        break;
    }
    keys[SDLK_RETURN] = 0;
  }

  if( keys[SDLK_LEFT]  ) {
    ProcessKey(SDLK_LEFT);
    keys[SDLK_LEFT] = 0;
  }

  if( keys[SDLK_RIGHT]  ) {
    ProcessKey(SDLK_RIGHT);
    keys[SDLK_RIGHT] = 0;
  }

  if( keys[SDLK_ESCAPE] ) {
     mParent->ToPage(&mParent->mainMenuPage);
     keys[SDLK_ESCAPE] = 0;
  }

  return 0;

}

void PageChangeSetup::ProcessKey(int keyCode) {
  
  int x;

  switch(keyCode) {

    // LEFT ----------------------------------------
    case SDLK_LEFT:

      switch( selItem ) {

        case 0: // Pit width
          x = mParent->GetSetup()->GetPitWidth();
          if( x>MIN_PITWIDTH )
            mParent->GetSetup()->SetPitWidth(x-1);
          else
            mParent->GetSetup()->SetPitWidth(MAX_PITWIDTH);
          break;
        case 1: // Pit height
          x = mParent->GetSetup()->GetPitHeight();
          if( x>MIN_PITHEIGHT )
            mParent->GetSetup()->SetPitHeight(x-1);
          else
            mParent->GetSetup()->SetPitHeight(MAX_PITHEIGHT);
          break;
        case 2: // Pit depth
          x = mParent->GetSetup()->GetPitDepth();
          if( x>MIN_PITDEPTH )
            mParent->GetSetup()->SetPitDepth(x-1);
          else
            mParent->GetSetup()->SetPitDepth(MAX_PITDEPTH);
          break;
        case 3: // Block set
          x = mParent->GetSetup()->GetBlockSet();
          if( x>BLOCKSET_FLAT )
            mParent->GetSetup()->SetBlockSet(x-1);
          else
            mParent->GetSetup()->SetBlockSet(BLOCKSET_EXTENDED);
          break;
      }
      break;

    // RIGHT ----------------------------------------
    case SDLK_RIGHT:

      switch( selItem ) {
        case 0: // Pit width
          x = mParent->GetSetup()->GetPitWidth();
          if( x<MAX_PITWIDTH )
            mParent->GetSetup()->SetPitWidth(x+1);
          else
            mParent->GetSetup()->SetPitWidth(MIN_PITWIDTH);
          break;
        case 1: // Pit height
          x = mParent->GetSetup()->GetPitHeight();
          if( x<MAX_PITHEIGHT )
            mParent->GetSetup()->SetPitHeight(x+1);
          else
            mParent->GetSetup()->SetPitHeight(MIN_PITHEIGHT);
          break;
        case 2: // Pit depth
          x = mParent->GetSetup()->GetPitDepth();
          if( x<MAX_PITDEPTH )
            mParent->GetSetup()->SetPitDepth(x+1);
          else
            mParent->GetSetup()->SetPitDepth(MIN_PITDEPTH);
          break;
        case 3: // Block set
          x = mParent->GetSetup()->GetBlockSet();
          if( x<BLOCKSET_EXTENDED )
            mParent->GetSetup()->SetBlockSet(x+1);
          else
            mParent->GetSetup()->SetBlockSet(BLOCKSET_FLAT);
          break;
      }
      break;

  }

}


