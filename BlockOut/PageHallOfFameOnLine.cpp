/*
   File:        PageHallOfFameOnLine.cpp
  Description: Hall of Fame page (From the on-line database)
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

void PageHallOfFameOnLine::Prepare(int iParam,void *pParam) {

  if( iParam == 0 ) {
    // Come from Main menu
    nbItem  = 99;
    selItem = iParam;
    startPos = 0;
    downloadState = 3;
  }

}

PLAYER_INFO *PageHallOfFameOnLine::GetPlayer() {

  strcpy(pInfo.name , allScore[selItem].name);
  int i = (int)strlen(pInfo.name) - 1;
  while(i>=0 && pInfo.name[i]==' ') {
    pInfo.name[i]=0;
    i--;
  }
  pInfo.highScore = allScore[0].score;
  pInfo.rank = selItem + 1;

  return &pInfo;

}

void PageHallOfFameOnLine::DownloadScore() {

  char link[512];

  // Retreive score from HTTP
  int timeout = mParent->GetSetup()->GetTimeout() * 1000;
  sprintf(link,"http://%s/getscore.php?setupid=%d&max=99",
    mParent->GetSetup()->GetHttpHome() ,
    mParent->GetSetup()->GetId() );

  char *page = mParent->GetHttp()->Get( link , timeout );
  if( page==NULL ) {

    downloadState = -1;
    strcpy(errMsg,mParent->GetHttp()->GetError());

  } else {

    // Got a response
    if(strncmp(page,"NbScore:",8)!=0) {

      downloadState = -1;
      sprintf(errMsg,"HTTP error");

    } else {

      int nb;
      sscanf(page+8,"%d",&nb);
      pPos = page;
      FetchNextLine();
      int i;
      for(i=0;i<nb;i++) {
        memset(allScore + i,0,sizeof(SCOREREC));
        ParseScore(allScore + i);
        FetchNextLine();
      }
      for(;i<100;i++) {
        memset(allScore + i,0,sizeof(SCOREREC));
        strcpy(allScore[i].name , ".........." );
      }

      downloadState = 0;

    }

  }

}

void PageHallOfFameOnLine::FetchNextLine() {

  while( ( (*pPos)!='\n' ) && ( (*pPos)!=0 ) ) pPos++;
  if( (*pPos) ) pPos++;

}

char *PageHallOfFameOnLine::FetchNextField() {

  static char ret[256];
  int i = 0;

  while( ( (*pPos)!='[' ) && ( (*pPos)!=0 ) ) pPos++;
  if( (*pPos) ) {
    pPos++;
    while( ( (*pPos)!=']' ) && ( (*pPos)!=0 ) ) {
      ret[i] = *pPos;
      pPos++;
      i++;
    }
    if( (*pPos) ) pPos++;
  }

  ret[i] = 0;
  return ret;

}

void PageHallOfFameOnLine::ParseScore(SCOREREC *s) {
  
  int x,h=0,m=0,sec=0;

  char *field = FetchNextField();
  sscanf(field,"%d",&(s->scoreId));
  field = FetchNextField();
  strncpy(s->name,field,10);
  (s->name)[10]=0;
  for(int i=(int)strlen(s->name);i<10;i++) (s->name)[i]=' ';
  field = FetchNextField();
  sscanf(field,"%d",&(s->score));
  field = FetchNextField();
  sscanf(field,"%d",&(s->nbCube));
  field = FetchNextField();
  sscanf(field,"%d",&x);
  s->emptyPit = (BYTE)x;
  field = FetchNextField();
  sscanf(field,"%d",&(s->nbLine1));
  field = FetchNextField();
  sscanf(field,"%d",&(s->nbLine2));
  field = FetchNextField();
  sscanf(field,"%d",&(s->nbLine3));
  field = FetchNextField();
  sscanf(field,"%d",&(s->nbLine4));
  field = FetchNextField();
  sscanf(field,"%d",&(s->nbLine5));
  field = FetchNextField();
  sscanf(field,"%d",&(s->startLevel));
  field = FetchNextField();
  sscanf(field,"%d",&(s->date));
  field = FetchNextField();
  field[2] = ' '; field[5] = ' ';
  sscanf(field,"%d %d %d",&h,&m,&sec);
  s->gameTime = (float)h * 3600.0f + (float)m * 60.f + (float)sec;

}

void PageHallOfFameOnLine::Render() {

  char tmp[256];
#ifdef PLATFORM_PSP
  char status;
  static char wifiButtonOff = 0;
#endif

  sprintf(tmp,"SCORE %s",mParent->GetSetup()->GetName());
  mParent->RenderTitle(tmp);

  // Online logo
  mParent->onlineLogo.Render();

  if( downloadState>1 ) {

#ifndef PLATFORM_PSP
    mParent->RenderText(0,0,FALSE,STR("Downloading..."));
    downloadState--;
#else
    if (psp.initWifi() != 0) {
        mParent->RenderText(0,0,FALSE,STR("WiFi initialization error..."));
    } else {
        status = psp.checkWifiButton();
        if (status != 0) {
            wifiButtonOff = 1;
            downloadState = 0;
        } else {
          mParent->RenderText(0,0,FALSE,STR("Connecting to WiFi network"));
          mParent->RenderText(0,1,FALSE,STR("Please wait..."));
        }
    }
    downloadState--;
#endif

  } else {

    // Download
    if( downloadState==1 ) {
#ifdef PLATFORM_PSP
        status = psp.connectToWifiNetwork();
        if (status != 0) {
            mParent->RenderText(0,1,FALSE,STR("Could not connecto to defined WiFi networks!"));
            downloadState = 0;
        }
#endif
      DownloadScore();
    }

    if( downloadState==0 ) {

      for(int i=0;i<10;i++) {

        int j = i + startPos;
        sprintf(tmp,"%2d ", j+1);
        mParent->RenderText(0,i,(selItem==j),tmp);
        mParent->RenderText(3,i,(selItem==j),allScore[j].name);
        sprintf(tmp,"%7d ",allScore[j].score);
        mParent->RenderText(13,i,(selItem==j),tmp);
        sprintf(tmp,"[%s]",FormatDateShort(allScore[j].date));
        mParent->RenderText(21,i,(selItem==j),tmp);

      }

    } else {

      // Error message
#ifdef PLATFORM_PSP
      if (wifiButtonOff) {
        mParent->RenderText(0,0,FALSE,STR("Turn WiFi button on..."));
      } else
#endif
      mParent->RenderText(0,0,FALSE,STR("Download error:"));
      int p = 0;
      int i = 1;
      while( strlen(errMsg+p) > 34 ) {
        strncpy(tmp,errMsg+p,34); tmp[34]=0;
        mParent->RenderText(0,i,FALSE,tmp);
        i++;
        p+=34;
      }
      mParent->RenderText(0,i,FALSE,errMsg+p);
    }

  }

}

int PageHallOfFameOnLine::Process(BYTE *keys,float fTime) {

  if( keys[SDLK_UP] ) {
    if( selItem>0 ) selItem--;
    keys[SDLK_UP]=0;
  }

  if( keys[SDLK_DOWN] ) {
    if( selItem<98 ) selItem++;
    keys[SDLK_DOWN]=0;
  }

  if( keys[SDLK_PAGEUP] ) {
    if( selItem>10 ) selItem-=10;
    else             selItem=0;
    keys[SDLK_PAGEUP]=0;
  }

  if( keys[SDLK_PAGEDOWN] ) {
    if( selItem<88 ) selItem+=10;
    else             selItem=98;
    keys[SDLK_PAGEDOWN]=0;
  }

  // Scroll to visible
  if( selItem<startPos ) {
    startPos = selItem;
  }

  if( selItem>=startPos+10 ) {
    startPos = selItem-9;
  }


  if( keys[SDLK_RETURN] ) {
    mParent->ToPage(&mParent->scoreDetailsPage,selItem + 0x100,allScore + selItem);
    keys[SDLK_RETURN] = 0;
  }

  if( keys[SDLK_ESCAPE] ) {
     mParent->ToPage(&mParent->mainMenuPage);
     keys[SDLK_ESCAPE] = 0;
  }

  return 0;

}
