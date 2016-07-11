/*
   File:        Sprites.cpp
  Description: Handles game sprites and fonts
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

#include "Sprites.h"

const float numberMap[] = { 0.002f , 0.107f , 0.170f , 0.271f , 0.369f , 0.486f , 0.588f , 0.689f , 0.791f , 0.893f , 0.994f };

// ----------------------------------------------------------------

Sprites::Sprites() {
}

// ----------------------------------------------------------------

int Sprites::Create(DWORD width,DWORD height) {

#ifndef PLATFORM_PSP
  if( !baseSprite.RestoreDeviceObjects(STR("images/sprites.png"),
                                       STR("images/spritesa.png"),
                                       width,height) )
    return GL_FAIL;

  if( !gameOverSprite.RestoreDeviceObjects(STR("images/gameover.png"),
                                           STR("images/gameovera.png"),
                                           width,height) )
    return GL_FAIL;
#else
  if( !baseSprite.RestoreDeviceObjects(STR("images.psp/sprites.png"),
                                       STR("images.psp/spritesa.png"),
                                       width,height) )
    return GL_FAIL;

  if( !gameOverSprite.RestoreDeviceObjects(STR("images.psp/gameover.png"),
                                           STR("images.psp/gameovera.png"),
                                           width,height) )
    return GL_FAIL;

  if( !pitLevelsSprite.RestoreDeviceObjects(STR("images.psp/levels.png"),
                                            STR("none"),
                                            width,height) )
    return GL_FAIL;
 #endif

  // Init coordinates
  xScore  = fround( 0.8369f * (float)width);
  yScore  = fround( 0.2500f * (float)height);
  wScore  = fround( 0.1416f * (float)width);
  hScore  = fround( 0.0470f * (float)height);
  xCube   = fround( 0.8369f * (float)width);
  yCube   = fround( 0.3828f * (float)height);
  xhScore = fround( 0.8369f * (float)width);
  yhScore = fround( 0.6575f * (float)height);
  xPit    = fround( 0.8369f * (float)width);
  yPit    = fround( 0.7942f * (float)height);
  xBlock  = fround( 0.8369f * (float)width);
  yBlock  = fround( 0.9297f * (float)height);

  xLevel  = fround( 0.0186f * (float)width);
  yLevel  = fround( 0.0651f * (float)height);
  wLevel  = fround( 0.0479f * (float)width);
  hLevel  = fround( 0.0635f * (float)height);

  xReplay = fround( 0.8301f * (float)width);
  yReplay = fround( 0.5078f * (float)height);
  wReplay = fround( 0.1543f * (float)width);
  hReplay = fround( 0.0456f * (float)height);

  xOnline = fround( 0.9150f * (float)width);
  yOnline = fround( 0.5534f * (float)height);
  wOnline = fround( 0.0684f * (float)width);
  hOnline = fround( 0.0534f * (float)height);

  xGOver  = fround( 0.3252f * (float)width);
  yGOver  = fround( 0.4167f * (float)height);
  wGOver  = fround( 0.2500f * (float)width);
  hGOver  = fround( 0.1680f * (float)height);

  // number font width
  for(int i=0;i<10;i++) {
    numberWidth[i] = (int)( (float)wScore * (numberMap[i+1] - numberMap[i]) * 1.55f );
  }

  return GL_OK;

}

// ----------------------------------------------------------------

void Sprites::InvalidateDeviceObjects() {
  baseSprite.InvalidateDeviceObjects();
  gameOverSprite.InvalidateDeviceObjects();
}

// ----------------------------------------------------------------

void Sprites::RenderScore(DWORD score,DWORD level,DWORD cubes) {

  char tmp[256];

  // Score
  sprintf(tmp,"%u",score);
  RenderNumbers(xScore,yScore,tmp);

  // Cube
  sprintf(tmp,"%u",cubes);
  RenderNumbers(xScore,yCube,tmp);

  // Level
  RenderLevel(level);

}

// ----------------------------------------------------------------

void Sprites::RenderInfo(DWORD hScore,int x,int y,int z,int blockSet) {

  char tmp[256];

  // High Score
  sprintf(tmp,"%u",hScore);
  RenderNumbers(xScore,yhScore,tmp);

  // Pit
  sprintf(tmp,"%ux%ux%u",x,y,z);
  RenderNumbers(xScore,yPit,tmp);

  // Set
  RenderBlockSet(blockSet);

}

// ----------------------------------------------------------------

int Sprites::GetNumberWidth(char number) {

  if( number>=0 && number<=9 ) {
    return numberWidth[number];
  } else {
    return numberWidth[8];
  }

}

// ----------------------------------------------------------------

void Sprites::RenderNumbers(int x,int y,char *strMumber) {

  float sX;
  float eX;
  float sY;
  float eY;
  int lgth = (int)strlen(strMumber);
  int strLength=0;
  int i,pos;
  char number;

  // String length
  for(i=0;i<lgth;i++) {
    number = strMumber[i] - '0';
    strLength += GetNumberWidth(number);
  }

  i = 0;
  pos = x + (wScore - strLength);

  // Clipping
  if( strLength>wScore ) {

    while( pos<x ) {
      number = strMumber[i] - '0';
      pos += GetNumberWidth(number);
      if(pos<x) i++;
    }

    int nwidth = (pos-x);

    if( nwidth>0 ) {

      // Adapt sprite mapping and coordinates
      if( number>=0 && number<=9 ) {
        float offset = (1.0f - (float)nwidth/(float)GetNumberWidth(number)) * (numberMap[number+1]-numberMap[number]);
        sX = numberMap[number] + offset;
        eX = numberMap[number+1];
        sY = 0.0f;
        eY = 0.125f;
      } else {
        float offset = (1.0f - (float)nwidth/(float)GetNumberWidth(8)) * 0.098f;
        sX = offset;
        eX = 0.098f;
        sY = 0.123f;
        eY = 0.222f;
      }
    
      // Update and render sprite
      baseSprite.UpdateSprite(x,y,x+nwidth,y+hScore,sX,sY,eX,eY);
      baseSprite.Render();

    }

    i++;

  }

  for(;i<lgth;i++) {

    number = strMumber[i] - '0';

    // Mapping corrdinates
    if( number>=0 && number<=9 ) {
      sX = numberMap[number];
      eX = numberMap[number+1];
      sY = 0.0f;
      eY = 0.125f;
    } else {
      // x
      sX = 0.0f;
      eX = 0.098f;
      sY = 0.123f;
      eY = 0.222f;
    }

    // Update and render sprite
    baseSprite.UpdateSprite(pos,y,pos+GetNumberWidth(number),y+hScore,sX,sY,eX,eY);
    baseSprite.Render();

    pos += GetNumberWidth(number);

  }

}

// ----------------------------------------------------------------

void Sprites::RenderLevel(int level) {

  float sX;
  float eX;
  float sY;
  float eY;

  // Mapping corrdinates
  if( level<=9 ) {
    sX = numberMap[level];
    eX = numberMap[level+1];
    sY = 0.473f;
    eY = 0.598f;
    int border = (wLevel - (numberWidth[level] * 2))/2;
    baseSprite.UpdateSprite(xLevel+border,yLevel,
                            xLevel+wLevel-border,yLevel+hLevel,
                            sX,sY,eX,eY);
  } else {
    // TDM
    sX = 0.100f;
    sY = 0.132f;
    eX = 0.215f;
    eY = 0.230f;
    baseSprite.UpdateSprite(xLevel,yLevel,
                            xLevel+wLevel,yLevel+hLevel,
                            sX,sY,eX,eY);
  }

  // Update and render sprite
  baseSprite.Render();

}

// ----------------------------------------------------------------

void Sprites::RenderGameMode(int mode) {

  // Mapping corrdinates
  float sX = 0.0f;
  float eX = 1.0f;
  float sY,eY;

  switch(mode) {
    case GAME_OVER:
      sY = 0.0f;
      eY = 0.5f;
      break;
    case GAME_PAUSED:
      sY = 0.5f;
      eY = 1.0f;
      break;
    default:
      return;
  }

  // Screen pos
  int x1 = xGOver;
  int y1 = yGOver;
  int x2 = xGOver + wGOver;
  int y2 = yGOver + hGOver;

  // Update and render sprite
  gameOverSprite.UpdateSprite(x1,y1,x2,y2,sX,sY,eX,eY);
  gameOverSprite.Render();

}

// ----------------------------------------------------------------

void Sprites::RenderReplay() {

  // Mapping
  float sX = 0.0000f;
  float eX = 0.6172f;
  float sY = 0.6093f;
  float eY = 0.7305f;

  // Screen pos
  int x1 = xReplay;
  int y1 = yReplay;
  int x2 = xReplay + wReplay;
  int y2 = yReplay + hReplay;

  // Update and render sprite
  baseSprite.UpdateSprite(x1,y1,x2,y2);
  baseSprite.SetSpriteMapping(sX,sY,eX,eY);
  baseSprite.Render();
}

// ----------------------------------------------------------------

void Sprites::RenderOnLine() {

  // Mapping
  float sX = 0.6445f;
  float sY = 0.2109f;
  float eX = 0.8984f;
  float eY = 0.3711f;

  // Screen pos
  int x1 = xOnline;
  int y1 = yOnline;
  int x2 = xOnline + wOnline;
  int y2 = yOnline + hOnline;

  // Update and render sprite
  baseSprite.UpdateSprite(x1,y1,x2,y2);
  baseSprite.SetSpriteMapping(sX,sY,eX,eY);
  baseSprite.Render();
}

// ----------------------------------------------------------------

void Sprites::RenderDemo() {

  // Mapping
  float sX = 0.0000f;
  float eX = 0.6172f;
  float sY = 0.7422f;
  float eY = 0.8594f;

  // Screen pos
  int x1 = xReplay;
  int y1 = yReplay;
  int x2 = xReplay + wReplay;
  int y2 = yReplay + hReplay;

  // Update and render sprite
  baseSprite.UpdateSprite(x1,y1,x2,y2);
  baseSprite.SetSpriteMapping(sX,sY,eX,eY);
  baseSprite.Render();
}

// ----------------------------------------------------------------

void Sprites::RenderPractice() {

  // Mapping
  float sX = 0.0000f;
  float eX = 0.6602f;
  float sY = 0.8594f;
  float eY = 0.9805f;

  // Screen pos
  int x1 = xReplay;
  int y1 = yReplay;
  int x2 = xReplay + wReplay;
  int y2 = yReplay + hReplay;

  // Update and render sprite
  baseSprite.UpdateSprite(x1,y1,x2,y2);
  baseSprite.SetSpriteMapping(sX,sY,eX,eY);
  baseSprite.Render();
}

// ----------------------------------------------------------------

void Sprites::RenderBlockSet(int blockSet) {

  float sX;
  float eX;
  float sY;
  float eY;

  // Mapping corrdinates
  sX = 0.0;
  eX = 0.578f;

  switch(blockSet) {
    case BLOCKSET_FLAT:
      sY = 0.391f;
      eY = 0.477f;
      break;
    case BLOCKSET_BASIC:
      sY = 0.309f;
      eY = 0.391f;
      break;
    case BLOCKSET_EXTENDED:
      sY = 0.227f;
      eY = 0.309f;
      break;
  }

  // Screen pos
  int x1 = xBlock;
  int y1 = yBlock;
  int x2 = xBlock + wScore;
  int y2 = yBlock + hScore;

  // Update and render sprite
  baseSprite.UpdateSprite(x1,y1,x2,y2);
  baseSprite.SetSpriteMapping(sX,sY,eX,eY);
  baseSprite.Render();

}

// ----------------------------------------------------------------

#ifdef PLATFORM_PSP
void Sprites::RenderPitLevels(int level) {
    int cubeNumber = 7;
    int x1 = 10;
    int y1 = 265;
    int x2 = 32;
    int height = 12;

    for (int i=0;i<level; i++) {
      pitLevelsSprite.UpdateSprite(x1,y1 - height*i,x2,y1 - height - height*i,0.43f,0.81f - 0.093f * (i % cubeNumber),0.58f,0.73f - 0.093f * (i % cubeNumber));
      pitLevelsSprite.Render();
    }

}
#endif
