/*
   File:        PageScoreDetails.cpp
  Description: Score Details menu page
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

void PageScoreDetails::Prepare(int iParam,void *pParam) {

  if(pParam) {
    nbItem  = 1;
    selItem = 0;
    score = (SCOREREC *)pParam;
    rank = iParam;
  }

}

void PageScoreDetails::Render() {

  char tmp[256];

  sprintf(tmp,"SCORE DETAILS [Rank %d]",(rank+1)&0xFF);
  mParent->RenderTitle(tmp);

  sprintf(tmp,"Setup: %s" , mParent->GetSetup()->GetName());
  mParent->RenderText(0,0,FALSE,tmp);
  sprintf(tmp,"Score: %d by %s" , score->score, score->name);
  mParent->RenderText(0,1,FALSE,tmp);
  sprintf(tmp,"Date : %s" , FormatDate(score->date));
  mParent->RenderText(0,2,FALSE,tmp);
  sprintf(tmp,"Time : %s" , FormatTime(score->gameTime));
  mParent->RenderText(0,3,FALSE,tmp);
  sprintf(tmp,"Cubes: %d" , score->nbCube);
  mParent->RenderText(0,4,FALSE,tmp);
  sprintf(tmp,"Level: From %d To " , score->startLevel);
  mParent->RenderText(0,5,FALSE,tmp);
  // Calculate end level
  int nbCubePerLevel = mParent->GetSetup()->GetPitWidth()*15 + mParent->GetSetup()->GetPitHeight()*15;
  int endLevel = score->nbCube / nbCubePerLevel;
  if( endLevel<10 ) {
    if( endLevel <= score->startLevel ) {
      sprintf(tmp,"%d" , score->startLevel);
    } else {
      sprintf(tmp,"%d" , endLevel);
    }
  } else {
    sprintf(tmp,"#");
  }
  mParent->RenderText(17,5,FALSE,tmp);
  sprintf(tmp,"Flush: %d",score->emptyPit);
  mParent->RenderText(0,6,FALSE,tmp);
  // Lines
  int nbLine =   score->nbLine1 + score->nbLine2*2 + score->nbLine3*3
               + score->nbLine4*4 + score->nbLine5*5;
  sprintf(tmp,"Lines: %3d %02d[x1] %02d[x2] %02d[x3]" , nbLine ,
                 score->nbLine1, score->nbLine2, score->nbLine3);
  mParent->RenderText(0,7,FALSE,tmp);
  sprintf(tmp,"           %02d[x4] %02d[x5]" , score->nbLine4, score->nbLine5);
  mParent->RenderText(0,8,FALSE,tmp);
  mParent->RenderText(0,9,(selItem==0),STR("Try to beat"));

}

int PageScoreDetails::Process(BYTE *keys,float fTime) {

  ProcessDefault(keys,fTime);

  if( keys[SDLK_RETURN] ) {
    mParent->ToPage(&mParent->startGamePage);
    keys[SDLK_RETURN] = 0;
  }

  if( keys[SDLK_ESCAPE] ) {
    if( rank & 0x100 ) {
      mParent->ToPage(&mParent->hallOfFamePageOnLine,1,NULL);
    } else {
      mParent->ToPage(&mParent->hallOfFamePage,rank,NULL);
    }
    keys[SDLK_ESCAPE] = 0;
  }

  return 0;
}
