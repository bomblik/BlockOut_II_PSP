/*
   File:        Menu.cpp
  Description: Menu management
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
#include <time.h>


// ---------------------------------------------------------------------

Menu::Menu() {
  
  setupManager = NULL;
  soundManager = NULL;
  for (int i=0;i<BLLETTER_NB;i++)
	  blLetters[i]=NULL;
  InitGraphics();
  CreatePage();
  ToPage(&mainMenuPage);

}

// ---------------------------------------------------------------------

Menu::~Menu() {
  for(int i=0;i<BLLETTER_NB;i++)
    if(blLetters[i]) delete blLetters[i];
}

// ---------------------------------------------------------------------

void Menu::SetSetupManager(SetupManager *manager) {
  setupManager = manager;
}

// ---------------------------------------------------------------------

SetupManager *Menu::GetSetup() {
  return setupManager;
}

// ---------------------------------------------------------------------

void Menu::SetSoundManager(SoundManager *manager) {
  soundManager = manager;
}

// ---------------------------------------------------------------------

SoundManager *Menu::GetSound() {
  return soundManager;
}

// ---------------------------------------------------------------------

void Menu::SetHttp(Http *h) {
  http = h;
}

// ---------------------------------------------------------------------

Http *Menu::GetHttp() {
  return http;
}

// ---------------------------------------------------------------------

int Menu::Process(BYTE *keys,float fTime) {

  int exitValue = 0;
  ProcessAnim(fTime);

  if( animEnded ) {

#ifndef PLATFORM_PSP
     // Limit frame 
    if( selPage != &controlsPage &&  selPage != &creditsPage ) {
      SDL_Delay(50);
    }
#endif
    exitValue = selPage->Process(keys,fTime);

  } else {

    // Fast animation ending
    if( keys[SDLK_ESCAPE] ) {
      startMenuTime = fTime - (ANIMTIME + BLLETTER_NB*0.25f);
      menuEscaped = TRUE;
      keys[SDLK_ESCAPE] = 0;
    }

  }

  return exitValue;

}

// ---------------------------------------------------------------------

void Menu::ToPage(MenuPage *page) {
  ToPage(page,0,NULL);
}

// ---------------------------------------------------------------------

void Menu::ToPage(MenuPage *page,int iParam,void *wParam) {

  selPage = page;
  selPage->Prepare(iParam,wParam);
  FullRepaint();
  if( soundManager )
    soundManager->PlayBlub();

}
