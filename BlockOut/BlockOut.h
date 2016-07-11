/*
   File:        BlockOut.h
  Description: Main application class
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

#include "Game.h"
#include "Menu.h"
#include "SetupManager.h"
#include "SoundManager.h"

#include "Http.h"

#define MENU_MODE 1
#define GAME_MODE 2

class BlockOut : public GLApplication
{

    GLFont2D m_pSmallFont;
	char extendedTitle[256];

    // Navigation
    BYTE m_bKey[SDLK_LAST];

    // Global mode
    BOOL inited;
    int  mode;
    int  lastSleepTime;

protected:
    int OneTimeSceneInit();
    int RestoreDeviceObjects();
    int InvalidateDeviceObjects();
    int Render();
    int FrameMove();
    int EventProc(SDL_Event *event);

    int UpdateFullScreen();

public:
    BlockOut();

    // Global handles
    Game theGame;
    Menu theMenu;
    SetupManager theSetup;
    SoundManager theSound;
    Http         theHttp;

};
