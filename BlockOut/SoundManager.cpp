/*
   File:        SoundManager.cpp
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

#include "SoundManager.h"
#include "Types.h"
#include <stdio.h>

#define PLAY(x) if( enabled && x ) Mix_PlayChannel (-1, x, 0);

// ------------------------------------------------

SoundManager::SoundManager() {

  enabled = FALSE;
  blubSound = NULL;
  wozzSound = NULL;
  tchhSound = NULL;
  lineSound = NULL;
  levelSound = NULL;
  wellDoneSound = NULL;
  emptySound = NULL;
  line2Sound = NULL;
  level2Sound = NULL;
  wellDone2Sound = NULL;
  empty2Sound = NULL;
  creditsMusic = NULL;
  hitSound = NULL;
  strcpy(errMsg,"");

}

// ------------------------------------------------

int SoundManager::Create() {

  if ( Mix_OpenAudio (44100, AUDIO_S16, 2, 512) < 0 )
  {
    sprintf(errMsg, "SDL_mixer: Audio initialisation error");
    enabled=FALSE;
    return FALSE;
  }  

  if( !InitSound(LID(STR("sounds/blub.wav")),&blubSound) ) return FALSE;
  if( !InitSound(LID(STR("sounds/wozz.wav")),&wozzSound) ) return FALSE;
  if( !InitSound(LID(STR("sounds/tchh.wav")),&tchhSound) ) return FALSE;
  if( !InitSound(LID(STR("sounds/line.wav")),&lineSound) ) return FALSE;
  if( !InitSound(LID(STR("sounds/level.wav")),&levelSound) ) return FALSE;
  if( !InitSound(LID(STR("sounds/empty.wav")),&emptySound) ) return FALSE;
  if( !InitSound(LID(STR("sounds/welldone.wav")),&wellDoneSound) ) return FALSE;
  if( !InitSound(LID(STR("sounds/line2.wav")),&line2Sound) ) return FALSE;
  if( !InitSound(LID(STR("sounds/level2.wav")),&level2Sound) ) return FALSE;
  if( !InitSound(LID(STR("sounds/empty2.wav")),&empty2Sound) ) return FALSE;
  if( !InitSound(LID(STR("sounds/welldone2.wav")),&wellDone2Sound) ) return FALSE;
  if( !InitSound(LID(STR("sounds/hit.wav")),&hitSound) ) return FALSE;

  // Demo music on channel 0
  Mix_ReserveChannels(1);

  return TRUE;

}

// ------------------------------------------------

void SoundManager::SetEnable(BOOL enable) {
  enabled = enable;
}

// ------------------------------------------------

int SoundManager::InitSound(char *fileName,Mix_Chunk **snd) {

  *snd = Mix_LoadWAV ( fileName );
  if( *snd == NULL ) {
    sprintf(errMsg, "Failed to initialise %s : %s",fileName,SDL_GetError());
  }
  return ( *snd != NULL );

}

// ------------------------------------------------

char *SoundManager::GetErrorMsg() {

  return errMsg;

}

// ------------------------------------------------

void SoundManager::PlayBlub() {
  PLAY(blubSound);
}
void SoundManager::PlayWozz() {
  PLAY(wozzSound);
}
void SoundManager::PlayTchh() {
  PLAY(tchhSound);
}
void SoundManager::PlayLine() {
  PLAY(lineSound);
}
void SoundManager::PlayLevel() {
  PLAY(levelSound);
}
void SoundManager::PlayEmpty() {
  PLAY(emptySound);
}
void SoundManager::PlayWellDone() {
  PLAY(wellDoneSound);
}
void SoundManager::PlayLine2() {
  PLAY(line2Sound);
}
void SoundManager::PlayLevel2() {
  PLAY(level2Sound);
}
void SoundManager::PlayEmpty2() {
  PLAY(empty2Sound);
}
void SoundManager::PlayWellDone2() {
  PLAY(wellDone2Sound);
}
void SoundManager::PlayHit() {
  PLAY(hitSound);
}

void SoundManager::PlayMusic() {

  if( creditsMusic==NULL ) {
    creditsMusic = Mix_LoadMUS( LID(STR("sounds/music.ogg")) );
    if( !creditsMusic ) {
#ifdef WINDOWS
    char message[256];
	sprintf(message,"Music cannot be played: %s\n",SDL_GetError());
	MessageBox(NULL,message,"Warning",MB_OK|MB_ICONWARNING);
#else
      printf("Music cannot be played: %s\n",SDL_GetError());
#endif
      return;
    }
  }
  if( creditsMusic ) {
    Mix_FadeInMusic(creditsMusic,-1,1000);
  }
  
}

void SoundManager::StopMusic() {

  if( creditsMusic ) {
    Mix_HaltMusic();
     Mix_FreeMusic(creditsMusic);
    creditsMusic = NULL;
  }
  
}
