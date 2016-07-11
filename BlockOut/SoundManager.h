/*
   File:        SoundManager.h
  Description: Sound management
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
#include "GLApp/GLApp.h"
#ifndef PLATFORM_PSP
#include <SDL_mixer.h>
#else
#include "SDL/SDL_mixer.h"
#endif

#ifndef SOUNDMANAGERH
#define SOUNDMANAGERH

class SoundManager {

  public:
    SoundManager();

    // Initialise the sound manager
    int Create();

    // Play sounds
    void PlayBlub();
    void PlayWozz();
    void PlayTchh();

    // Game sounds
    void PlayLine();
    void PlayLevel();
    void PlayEmpty();
    void PlayWellDone();
    void PlayLine2();
    void PlayLevel2();
    void PlayEmpty2();
    void PlayWellDone2();
    void PlayHit();

    // Demo music
    void PlayMusic();
    void StopMusic();

    // Get error message
    char *GetErrorMsg();

    // Enable/Disable sound
    void SetEnable(BOOL enable);

  private:
    int InitSound(char *fileName,Mix_Chunk **snd);

    char errMsg[1024];
    BOOL enabled;

    Mix_Chunk*       blubSound;
    Mix_Chunk*       wozzSound;
    Mix_Chunk*       tchhSound;
    Mix_Chunk*       lineSound;
    Mix_Chunk*       levelSound;
    Mix_Chunk*       wellDoneSound;
    Mix_Chunk*       emptySound;
    Mix_Chunk*       line2Sound;
    Mix_Chunk*       level2Sound;
    Mix_Chunk*       wellDone2Sound;
    Mix_Chunk*       empty2Sound;
    Mix_Chunk*       hitSound;

    Mix_Music*       creditsMusic;

};

#endif /* SOUNDMANAGERH */
