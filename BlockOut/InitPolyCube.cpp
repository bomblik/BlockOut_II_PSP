/*
   File:        InitPolyCube.cpp
  Description: Initialise the 41 polycubes of the game
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
#ifdef AI_TEST

#include "BotPlayer.h"
void BotPlayer::InitPolyCube() {

#else

#include "Game.h"
int Game::InitPolyCube(BOOL transparent,float wEdge) {

  int hr;

#endif

  // Initialise polycube orientations (needed by the AI player)
  #include "BlockOrientation.h"

  allPolyCube[0].AddCube(0,0,0);
  allPolyCube[0].SetInfo(156,14,TRUE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[0].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[1].AddCube(0,0,0);
  allPolyCube[1].AddCube(0,1,0);
  allPolyCube[1].SetInfo(156,14,TRUE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[1].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[2].AddCube(0,0,0);
  allPolyCube[2].AddCube(0,1,0);
  allPolyCube[2].AddCube(0,2,0);
  allPolyCube[2].SetInfo(307,27,TRUE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[2].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[3].AddCube(0,0,0);
  allPolyCube[3].AddCube(0,1,0);
  allPolyCube[3].AddCube(0,2,0);
  allPolyCube[3].AddCube(0,3,0);
  allPolyCube[3].SetInfo(307,27,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[3].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[4].AddCube(0,0,0);
  allPolyCube[4].AddCube(0,1,0);
  allPolyCube[4].AddCube(0,2,0);
  allPolyCube[4].AddCube(0,3,0);
  allPolyCube[4].AddCube(0,4,0);
  allPolyCube[4].SetInfo(624,53,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[4].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[5].AddCube(0,0,0);
  allPolyCube[5].AddCube(1,0,0);
  allPolyCube[5].AddCube(1,1,0);
  allPolyCube[5].SetInfo(307,27,TRUE,TRUE);
#ifndef AI_TEST
  hr = allPolyCube[5].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[6].AddCube(0,0,0);
  allPolyCube[6].AddCube(1,0,0);
  allPolyCube[6].AddCube(0,1,0);
  allPolyCube[6].AddCube(1,1,0);
  allPolyCube[6].SetInfo(156,14,TRUE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[6].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[7].AddCube(0,0,0);
  allPolyCube[7].AddCube(1,0,0);
  allPolyCube[7].AddCube(2,0,0);
  allPolyCube[7].AddCube(1,1,0);
  allPolyCube[7].SetInfo(461,40,TRUE,TRUE);
#ifndef AI_TEST
  hr = allPolyCube[7].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[8].AddCube(1,0,0);
  allPolyCube[8].AddCube(2,0,0);
  allPolyCube[8].AddCube(0,1,0);
  allPolyCube[8].AddCube(1,1,0);
  allPolyCube[8].SetInfo(461,40,TRUE,TRUE);
#ifndef AI_TEST
  hr = allPolyCube[8].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[9].AddCube(0,0,0);
  allPolyCube[9].AddCube(1,0,0);
  allPolyCube[9].AddCube(2,0,0);
  allPolyCube[9].AddCube(2,1,0);
  allPolyCube[9].SetInfo(307,27,TRUE,TRUE);
#ifndef AI_TEST
  hr = allPolyCube[9].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[10].AddCube(0,0,0);
  allPolyCube[10].AddCube(1,0,0);
  allPolyCube[10].AddCube(2,0,0);
  allPolyCube[10].AddCube(2,1,0);
  allPolyCube[10].AddCube(2,2,0);
  allPolyCube[10].SetInfo(921,79,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[10].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[11].AddCube(2,0,0);
  allPolyCube[11].AddCube(0,1,0);
  allPolyCube[11].AddCube(1,1,0);
  allPolyCube[11].AddCube(2,1,0);
  allPolyCube[11].AddCube(2,2,0);
  allPolyCube[11].SetInfo(921,79,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[11].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[12].AddCube(2,0,0);
  allPolyCube[12].AddCube(0,1,0);
  allPolyCube[12].AddCube(1,1,0);
  allPolyCube[12].AddCube(2,1,0);
  allPolyCube[12].AddCube(1,2,0);
  allPolyCube[12].SetInfo(921,79,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[12].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[13].AddCube(1,0,0);
  allPolyCube[13].AddCube(2,0,0);
  allPolyCube[13].AddCube(1,1,0);
  allPolyCube[13].AddCube(0,2,0);
  allPolyCube[13].AddCube(1,2,0);
  allPolyCube[13].SetInfo(921,79,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[13].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[14].AddCube(0,0,0);
  allPolyCube[14].AddCube(1,0,0);
  allPolyCube[14].AddCube(1,1,0);
  allPolyCube[14].AddCube(2,1,0);
  allPolyCube[14].AddCube(2,2,0);
  allPolyCube[14].SetInfo(780,66,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[14].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[15].AddCube(0,0,0);
  allPolyCube[15].AddCube(1,0,0);
  allPolyCube[15].AddCube(1,1,0);
  allPolyCube[15].AddCube(1,2,0);
  allPolyCube[15].AddCube(1,3,0);
  allPolyCube[15].SetInfo(780,66,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[15].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[16].AddCube(1,0,0);
  allPolyCube[16].AddCube(0,1,0);
  allPolyCube[16].AddCube(1,1,0);
  allPolyCube[16].AddCube(1,2,0);
  allPolyCube[16].AddCube(1,3,0);
  allPolyCube[16].SetInfo(921,79,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[16].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[17].AddCube(0,0,0);
  allPolyCube[17].AddCube(1,0,0);
  allPolyCube[17].AddCube(1,1,0);
  allPolyCube[17].AddCube(1,2,0);
  allPolyCube[17].AddCube(0,2,0);
  allPolyCube[17].SetInfo(1248,105,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[17].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[18].AddCube(0,0,0);
  allPolyCube[18].AddCube(1,0,0);
  allPolyCube[18].AddCube(0,1,0);
  allPolyCube[18].AddCube(1,1,0);
  allPolyCube[18].AddCube(1,2,0);
  allPolyCube[18].SetInfo(461,40,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[18].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[19].AddCube(1,0,0);
  allPolyCube[19].AddCube(1,1,0);
  allPolyCube[19].AddCube(0,1,0);
  allPolyCube[19].AddCube(0,2,0);
  allPolyCube[19].AddCube(0,3,0);
  allPolyCube[19].SetInfo(921,79,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[19].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[20].AddCube(1,0,0);
  allPolyCube[20].AddCube(0,1,0);
  allPolyCube[20].AddCube(1,1,0);
  allPolyCube[20].AddCube(2,1,0);
  allPolyCube[20].AddCube(1,2,0);
  allPolyCube[20].SetInfo(1402,118,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[20].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[21].AddCube(0,0,0);
  allPolyCube[21].AddCube(0,0,1);
  allPolyCube[21].AddCube(1,0,1);
  allPolyCube[21].AddCube(1,1,1);
  allPolyCube[21].AddCube(1,2,1);
  allPolyCube[21].SetInfo(1379,131,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[21].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[22].AddCube(1,0,0);
  allPolyCube[22].AddCube(0,0,1);
  allPolyCube[22].AddCube(1,0,1);
  allPolyCube[22].AddCube(0,1,1);
  allPolyCube[22].AddCube(0,2,1);
  allPolyCube[22].SetInfo(1379,131,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[22].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[23].AddCube(0,1,0);
  allPolyCube[23].AddCube(0,1,1);
  allPolyCube[23].AddCube(1,0,1);
  allPolyCube[23].AddCube(1,1,1);
  allPolyCube[23].AddCube(1,2,1);
  allPolyCube[23].SetInfo(965,92,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[23].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[24].AddCube(1,1,0);
  allPolyCube[24].AddCube(1,0,1);
  allPolyCube[24].AddCube(1,1,1);
  allPolyCube[24].AddCube(1,2,1);
  allPolyCube[24].AddCube(0,2,1);
  allPolyCube[24].SetInfo(965,92,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[24].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[25].AddCube(1,1,0);
  allPolyCube[25].AddCube(0,0,1);
  allPolyCube[25].AddCube(1,0,1);
  allPolyCube[25].AddCube(1,1,1);
  allPolyCube[25].AddCube(1,2,1);
  allPolyCube[25].SetInfo(965,92,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[25].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[26].AddCube(1,0,0);
  allPolyCube[26].AddCube(1,0,1);
  allPolyCube[26].AddCube(1,1,1);
  allPolyCube[26].AddCube(0,1,1);
  allPolyCube[26].AddCube(0,2,1);
  allPolyCube[26].SetInfo(1379,131,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[26].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[27].AddCube(0,0,0);
  allPolyCube[27].AddCube(0,0,1);
  allPolyCube[27].AddCube(0,1,1);
  allPolyCube[27].AddCube(1,1,1);
  allPolyCube[27].AddCube(1,2,1);
  allPolyCube[27].SetInfo(1379,131,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[27].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[28].AddCube(1,1,0);
  allPolyCube[28].AddCube(1,2,0);
  allPolyCube[28].AddCube(0,0,1);
  allPolyCube[28].AddCube(0,1,1);
  allPolyCube[28].AddCube(1,1,1);
  allPolyCube[28].SetInfo(1103,105,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[28].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[29].AddCube(0,1,0);
  allPolyCube[29].AddCube(0,2,0);
  allPolyCube[29].AddCube(1,0,1);
  allPolyCube[29].AddCube(1,1,1);
  allPolyCube[29].AddCube(0,1,1);
  allPolyCube[29].SetInfo(1103,105,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[29].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[30].AddCube(1,0,0);
  allPolyCube[30].AddCube(1,0,1);
  allPolyCube[30].AddCube(1,1,1);
  allPolyCube[30].AddCube(1,2,1);
  allPolyCube[30].AddCube(0,2,1);
  allPolyCube[30].SetInfo(1379,131,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[30].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[31].AddCube(0,0,0);
  allPolyCube[31].AddCube(0,0,1);
  allPolyCube[31].AddCube(0,1,1);
  allPolyCube[31].AddCube(0,2,1);
  allPolyCube[31].AddCube(1,2,1);
  allPolyCube[31].SetInfo(1379,131,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[31].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[32].AddCube(1,0,0);
  allPolyCube[32].AddCube(1,0,1);
  allPolyCube[32].AddCube(0,0,1);
  allPolyCube[32].AddCube(0,1,1);
  allPolyCube[32].SetInfo(552,53,FALSE,TRUE);
#ifndef AI_TEST
  hr = allPolyCube[32].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[33].AddCube(1,0,0);
  allPolyCube[33].AddCube(1,0,1);
  allPolyCube[33].AddCube(0,1,1);
  allPolyCube[33].AddCube(1,1,1);
  allPolyCube[33].SetInfo(552,53,FALSE,TRUE);
#ifndef AI_TEST
  hr = allPolyCube[33].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[34].AddCube(1,0,0);
  allPolyCube[34].AddCube(0,0,1);
  allPolyCube[34].AddCube(1,0,1);
  allPolyCube[34].AddCube(1,1,1);
  allPolyCube[34].SetInfo(552,53,FALSE,TRUE);
#ifndef AI_TEST
  hr = allPolyCube[34].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[35].AddCube(1,1,0);
  allPolyCube[35].AddCube(0,1,1);
  allPolyCube[35].AddCube(1,1,1);
  allPolyCube[35].AddCube(1,0,1);
  allPolyCube[35].AddCube(1,2,1);
  allPolyCube[35].SetInfo(827,79,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[35].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[36].AddCube(1,0,0);
  allPolyCube[36].AddCube(0,0,1);
  allPolyCube[36].AddCube(1,0,1);
  allPolyCube[36].AddCube(0,1,1);
  allPolyCube[36].AddCube(1,1,1);
  allPolyCube[36].SetInfo(461,40,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[36].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[37].AddCube(1,0,0);
  allPolyCube[37].AddCube(0,0,1);
  allPolyCube[37].AddCube(1,0,1);
  allPolyCube[37].AddCube(1,1,1);
  allPolyCube[37].AddCube(1,2,1);
  allPolyCube[37].SetInfo(1103,105,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[37].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[38].AddCube(1,0,0);
  allPolyCube[38].AddCube(1,1,0);
  allPolyCube[38].AddCube(0,1,1);
  allPolyCube[38].AddCube(1,1,1);
  allPolyCube[38].AddCube(1,2,1);
  allPolyCube[38].SetInfo(1103,105,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[38].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[39].AddCube(1,1,0);
  allPolyCube[39].AddCube(1,2,0);
  allPolyCube[39].AddCube(1,0,1);
  allPolyCube[39].AddCube(1,1,1);
  allPolyCube[39].AddCube(0,1,1);
  allPolyCube[39].SetInfo(1103,105,FALSE,FALSE);
#ifndef AI_TEST
  hr = allPolyCube[39].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
#endif

  allPolyCube[40].AddCube(0,0,0);
  allPolyCube[40].AddCube(1,1,0);
  allPolyCube[40].AddCube(0,0,1);
  allPolyCube[40].AddCube(0,1,1);
  allPolyCube[40].AddCube(1,1,1);
  allPolyCube[40].SetInfo(1379,131,FALSE,FALSE);

#ifndef AI_TEST
  hr = allPolyCube[40].Create(thePit.GetCubeSide(),thePit.GetOrigin(),transparent,wEdge);
  if(!hr) return GL_FAIL;
  return GL_OK;
#else
  for(int i=0;i<NB_POLYCUBE;i++)
    allPolyCube[i].InitRotationCenter();
  return GL_OK;
#endif

}

