/*
   File:        PageOptions.cpp
  Description: Graphic and sound options page
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

void PageGSOptions::Prepare(int iParam,void *pParam) {
  nbItem  = 9;
  selItem = 0;
}

void PageGSOptions::Render() {
  
  int size;

  mParent->RenderTitle(STR("GRAPHICS & SOUND"));

  mParent->RenderText(0,0,(selItem==0),STR("Sound            :"));
  if( mParent->GetSetup()->GetSound() )
    mParent->RenderText(19,0,FALSE,STR("Enabled"));
  else
    mParent->RenderText(19,0,FALSE,STR("Disabled"));
  mParent->RenderText(0,1,(selItem==1),  STR("FullScreen       :"));
  if( mParent->GetSetup()->GetFullScreen() ) {
    mParent->RenderText(19,1,FALSE,STR("Enabled"));
    mParent->RenderText(0,2,(selItem==2),STR("Screen Size      :"));
  } else {
    mParent->RenderText(19,1,FALSE,STR("Disabled"));
    mParent->RenderText(0,2,(selItem==2),STR("Window Size      :"));
  }
#ifndef PLATFORM_PSP
  size = mParent->GetSetup()->GetWindowSize();
  mParent->RenderText(19,2,FALSE,(char *)mParent->GetSetup()->GetResName(size));  
#else
  mParent->RenderText(19,2,FALSE,"480x272");
#endif

  mParent->RenderText(0,3,(selItem==3),STR("Rotation Speed   :"));
  mParent->RenderText(19,3,FALSE,STR("S[           ]F"));
  int aSpeed = mParent->GetSetup()->GetAnimationSpeed();
  int i;
  for(i=0;i<=aSpeed;i++)
    mParent->RenderText(21+i,3,TRUE,STR("."));
  for(;i<=ASPEED_FAST;i++)
    mParent->RenderText(21+i,3,FALSE,STR("."));

  mParent->RenderText(0,4,(selItem==4),STR("Transparent face :"));
  mParent->RenderText(19,4,FALSE,STR("T[           ]O"));
  int fTrans = mParent->GetSetup()->GetTransparentFace();
  for(i=0;i<=fTrans;i++)
    mParent->RenderText(21+i,4,TRUE,STR("."));
  for(;i<=FTRANS_MAX;i++)
    mParent->RenderText(21+i,4,FALSE,STR("."));

  mParent->RenderText(0,5,(selItem==5),STR("Style            :"));
  mParent->RenderText(19,5,FALSE,(char *)mParent->GetSetup()->GetStyleName());

  mParent->RenderText(0,6,(selItem==6),STR("Sound preset     :"));
  mParent->RenderText(19,6,FALSE,(char *)mParent->GetSetup()->GetSoundTypeName());

  mParent->RenderText(0,7,(selItem==7),STR("Frame limiter    :"));
  mParent->RenderText(19,7,FALSE,(char *)mParent->GetSetup()->GetFrLimitName());

  mParent->RenderText(0,8,(selItem==8),STR("Line width       :"));
  mParent->RenderText(19,8,FALSE,STR("T[           ]L"));
  int lWidth = mParent->GetSetup()->GetLineWidth();
  for(i=0;i<=lWidth;i++)
    mParent->RenderText(21+i,8,TRUE,STR("."));
  for(;i<=FTRANS_MAX;i++)
    mParent->RenderText(21+i,8,FALSE,STR("."));

}

int PageGSOptions::Process(BYTE *keys,float fTime) {

  int exitValue = 0;

  ProcessDefault(keys,fTime);

  if( keys[SDLK_RETURN] ) {
    switch( selItem ) {
      case 0: // Sound
      case 1: // FullScreen mode
      case 2: // Window size
      case 3: // Rotation speed
      case 4: // Transparent face
      case 5: // Style
      case 6: // Sound preset
      case 7: // Frame limiter
      case 8: // Line width
        exitValue = ProcessKey(SDLK_RIGHT);
        break;
    }
    keys[SDLK_RETURN] = 0;
  }

  if( keys[SDLK_LEFT]  ) {
    exitValue = ProcessKey(SDLK_LEFT);
    keys[SDLK_LEFT] = 0;
  }

  if( keys[SDLK_RIGHT]  ) {
    exitValue = ProcessKey(SDLK_RIGHT);
    keys[SDLK_RIGHT] = 0;
  }

  if( keys[SDLK_ESCAPE] ) {
     mParent->ToPage(&mParent->mainMenuPage);
     keys[SDLK_ESCAPE] = 0;
  }

  return exitValue;
}

int PageGSOptions::ProcessKey(int key) {

   int x;
   int exitValue = 0;

    switch( selItem ) {
      case 0: // Sound
        if( key==SDLK_RIGHT || key==SDLK_LEFT ) {
          BOOL sound = mParent->GetSetup()->GetSound();
          mParent->GetSetup()->SetSound(!sound);
          mParent->GetSound()->SetEnable(!sound);
          mParent->GetSound()->PlayBlub();
        }
      break;
      case 1: // Fullscreen mode
#ifndef PLATFORM_PSP
        if( key==SDLK_RIGHT || key==SDLK_LEFT ) {
          BOOL fs = mParent->GetSetup()->GetFullScreen();
          mParent->GetSetup()->SetFullScreen(!fs);
          exitValue = 3; // Update full screen
        }
#endif
      break;
      case 2: // Window size
#ifndef PLATFORM_PSP
        switch( key ) {
          case SDLK_RIGHT:
            x = mParent->GetSetup()->GetWindowSize();
            if( x<RES_1600x1200 )
              mParent->GetSetup()->SetWindowSize(x+1);
            else
              mParent->GetSetup()->SetWindowSize(RES_640x480);
            exitValue = 2; // Resize
            break;
          case SDLK_LEFT:
            x = mParent->GetSetup()->GetWindowSize();
            if( x>RES_640x480 )
              mParent->GetSetup()->SetWindowSize(x-1);
            else
              mParent->GetSetup()->SetWindowSize(RES_1600x1200);
            exitValue = 2; // Resize
            break;
        }
#endif
      break;
      case 3: // Rotation speed
        switch( key ) {
          case SDLK_RIGHT:
            x = mParent->GetSetup()->GetAnimationSpeed();
            if( x<ASPEED_FAST )
              mParent->GetSetup()->SetAnimationSpeed(x+1);
            else
              mParent->GetSetup()->SetAnimationSpeed(ASPEED_SLOW);
            break;
          case SDLK_LEFT:
            x = mParent->GetSetup()->GetAnimationSpeed();
            if( x>ASPEED_SLOW )
              mParent->GetSetup()->SetAnimationSpeed(x-1);
            else
              mParent->GetSetup()->SetAnimationSpeed(ASPEED_FAST);
            break;
        }
      break;
      case 4: // Transparent face
        switch( key ) {
          case SDLK_RIGHT:
            x = mParent->GetSetup()->GetTransparentFace();
            if( x<FTRANS_MAX )
              mParent->GetSetup()->SetTransparentFace(x+1);
            else
              mParent->GetSetup()->SetTransparentFace(FTRANS_MIN);
            break;
          case SDLK_LEFT:
            x = mParent->GetSetup()->GetTransparentFace();
            if( x>FTRANS_MIN )
              mParent->GetSetup()->SetTransparentFace(x-1);
            else
              mParent->GetSetup()->SetTransparentFace(FTRANS_MAX);
            break;
        }
      break;
      case 5: // Game style
#ifndef PLATFORM_PSP
        switch( key ) {
          case SDLK_RIGHT:
            x = mParent->GetSetup()->GetStyle();
            if( x<STYLE_ARCADE )
              mParent->GetSetup()->SetStyle(x+1);
            else
              mParent->GetSetup()->SetStyle(STYLE_CLASSIC);
            break;
          case SDLK_LEFT:
            x = mParent->GetSetup()->GetStyle();
            if( x>STYLE_CLASSIC )
              mParent->GetSetup()->SetStyle(x-1);
            else
              mParent->GetSetup()->SetStyle(STYLE_ARCADE);
            break;
        }
#endif
      break;
      case 6: // Sound preset
        switch( key ) {
          case SDLK_RIGHT:
            x = mParent->GetSetup()->GetSoundType();
            if( x<SOUND_BLOCKOUT )
              mParent->GetSetup()->SetSoundType(x+1);
            else
              mParent->GetSetup()->SetSoundType(SOUND_BLOCKOUT2);
            break;
          case SDLK_LEFT:
            x = mParent->GetSetup()->GetSoundType();
            if( x>SOUND_BLOCKOUT2 )
              mParent->GetSetup()->SetSoundType(x-1);
            else
              mParent->GetSetup()->SetSoundType(SOUND_BLOCKOUT);
            break;
        }
      break;
      case 7: // Frame limiter
        switch( key ) {
          case SDLK_RIGHT:
            x = mParent->GetSetup()->GetFrLimiter();
            if( x<FR_LIMITVSYNC )
              mParent->GetSetup()->SetFrLimiter(x+1);
            else
              mParent->GetSetup()->SetFrLimiter(FR_NOLIMIT);
            break;
          case SDLK_LEFT:
            x = mParent->GetSetup()->GetFrLimiter();
            if( x>FR_NOLIMIT )
              mParent->GetSetup()->SetFrLimiter(x-1);
            else
              mParent->GetSetup()->SetFrLimiter(FR_LIMITVSYNC);
            break;
        }
      break;      
      case 8: // Line width
        switch( key ) {
          case SDLK_RIGHT:
            x = mParent->GetSetup()->GetLineWidth();
            if( x<LINEW_MAX )
              mParent->GetSetup()->SetLineWidth(x+1);
            else
              mParent->GetSetup()->SetLineWidth(LINEW_MIN);
            break;
          case SDLK_LEFT:
            x = mParent->GetSetup()->GetLineWidth();
            if( x>LINEW_MIN )
              mParent->GetSetup()->SetLineWidth(x-1);
            else
              mParent->GetSetup()->SetLineWidth(LINEW_MAX);
            break;
        }
        break;

    }

    return exitValue;

}
