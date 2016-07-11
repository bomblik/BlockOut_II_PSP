/*
   File:        PageHttp.cpp
  Description: HTTP config page
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

void PageHttp::Prepare(int iParam,void *pParam) {

  nbItem  = 5;
  selItem = 0;
  edit.SetDisplayLength(22);
  edit.SetMode(STR(""),FALSE,NULL);

}

char *PageHttp::Wrap(char *str) {

  static char ret[256];
  strcpy(ret,str);

  if( strlen(str)>21 ) {
    ret[21] = '{';
    ret[22] = 0;
  }

  return ret;

}

void PageHttp::Render() {
  
  char tmp[256];
  BOOL editMode = edit.GetMode();

  mParent->RenderTitle(STR("HTTP OPTIONS"));

  mParent->RenderText(0,0,(selItem==0) && !editMode,STR("Home      :"));
  if( !editMode || selItem!=0 ) {
    mParent->RenderText(12,0,FALSE,Wrap(mParent->GetSetup()->GetHttpHome()));
  }

  mParent->RenderText(0,1,(selItem==1) && !editMode,STR("Timeout[s]:"));
  if( !editMode || selItem!=1 ) {
    sprintf(tmp,"%d",mParent->GetSetup()->GetTimeout());
    mParent->RenderText(12,1,FALSE,tmp);
  }

  mParent->RenderText(0,2,(selItem==2),STR("HTTP proxy:"));
  if( mParent->GetSetup()->GetUseProxy() ) { 
    mParent->RenderText(12,2,FALSE,STR("Enabled"));
  } else {
    mParent->RenderText(12,2,FALSE,STR("Disabled"));
  }

  mParent->RenderText(0,3,(selItem==3) && !editMode,STR("Proxy host:"));
  if( !editMode || selItem!=3 ) {
    mParent->RenderText(12,3,FALSE,Wrap(mParent->GetSetup()->GetProxyName()));
  }

  mParent->RenderText(0,4,(selItem==4) && !editMode,STR("Proxy port:"));
  if( !editMode || selItem!=4 ) {
    sprintf(tmp,"%d",mParent->GetSetup()->GetProxyPort());
    mParent->RenderText(12,4,FALSE,tmp);
  }

  // Edition mode
  edit.Render(mParent,12,selItem);

}

int PageHttp::Process(BYTE *keys,float fTime) {
  
  BOOL b;
  char tmp[256];

  if( !edit.GetMode() ) {

    ProcessDefault(keys,fTime);

    if( keys[SDLK_RETURN] ) {
      switch(selItem) {
        case 0: // Edit Home
          strcpy(tmp,mParent->GetSetup()->GetHttpHome());
          edit.SetMode(tmp,TRUE,keys);
          break;
        case 1: // Edit timeout
          sprintf(tmp,"%d",mParent->GetSetup()->GetTimeout());
          edit.SetMode(tmp,TRUE,keys);
          break;
        case 3: // Edit proxy name
          strcpy(tmp,mParent->GetSetup()->GetProxyName());
          edit.SetMode(tmp,TRUE,keys);
          break;
        case 4: // Edit proxy port
          sprintf(tmp,"%d",mParent->GetSetup()->GetProxyPort());
          edit.SetMode(tmp,TRUE,keys);
          break;
        case 2: // Enable/Disable proxy
          b = mParent->GetSetup()->GetUseProxy();
          mParent->GetSetup()->SetUseProxy(!b);
          mParent->GetHttp()->SetProxy(!b);
          break;
      }
      keys[SDLK_RETURN] = 0;
    }

    if( keys[SDLK_ESCAPE] ) {
       mParent->ToPage(&mParent->optionsPage);
       keys[SDLK_ESCAPE] = 0;
    }

    if( keys[SDLK_LEFT] || keys[SDLK_RIGHT] ) {
      if( selItem==2 ) {
        b = mParent->GetSetup()->GetUseProxy();
        mParent->GetSetup()->SetUseProxy(!b);
        mParent->GetHttp()->SetProxy(!b);
      }
      keys[SDLK_LEFT] = 0;
      keys[SDLK_RIGHT] = 0;
    }

  } else {
    
    if( edit.Process(keys,fTime)==1 ) {

      // RETURN pressed
      int x=0;

      switch(selItem) {
        case 0: // Home
          mParent->GetSetup()->SetHttpHome(edit.GetText());
          break;
        case 1: // timeout
          sscanf(edit.GetText(),"%d",&x);
          mParent->GetSetup()->SetTimeout(x);
          break;
        case 3: // proxy name
          mParent->GetSetup()->SetProxyName(edit.GetText());
          mParent->GetHttp()->SetProxyName(edit.GetText());
          break;
        case 4: // proxy port
          sscanf(edit.GetText(),"%d",&x);
          mParent->GetSetup()->SetProxyPort(x);
          mParent->GetHttp()->SetProxyPort(x);
          break;
      }

    }

  }

  return 0;

}
