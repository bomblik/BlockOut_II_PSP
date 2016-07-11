/*
   File:        SetupManager.cpp
  Description: Setup management
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

#include "SetupManager.h"
#include <stdio.h>

// Converts the 975 possible game configurations to 585 configurations.
// (By considering identical 2 configurations c1 and c2, where 
// c1.width = c2.height and c1.height = c2.width)
// Index of the array corresponds to b*325 + (d-6)*25 + (w-3)*5 + (h-3)
// where b = blockSet (0 to 2) , d = Depth (6 to 18) , w = Width (3 to 7) 
// and h = Height (3 to 7)
const int setupId[] = {
      0,  1,  2,  4,  6,  1,  3,  5,  7,  9,  2,  5,  8, 10, 11,  4,  7, 10,
     12, 13,  6,  9, 11, 13, 14, 15, 16, 17, 19, 21, 16, 18, 20, 22, 24, 17,
     20, 23, 25, 26, 19, 22, 25, 27, 28, 21, 24, 26, 28, 29, 30, 31, 32, 34,
     36, 31, 33, 35, 37, 39, 32, 35, 38, 40, 41, 34, 37, 40, 42, 43, 36, 39,
     41, 43, 44, 45, 46, 47, 49, 51, 46, 48, 50, 52, 54, 47, 50, 53, 55, 56,
     49, 52, 55, 57, 58, 51, 54, 56, 58, 59, 60, 61, 62, 64, 66, 61, 63, 65,
     67, 69, 62, 65, 68, 70, 71, 64, 67, 70, 72, 73, 66, 69, 71, 73, 74, 75,
     76, 77, 79, 81, 76, 78, 80, 82, 84, 77, 80, 83, 85, 86, 79, 82, 85, 87,
     88, 81, 84, 86, 88, 89, 90, 91, 92, 94, 96, 91, 93, 95, 97, 99, 92, 95,
     98,100,101, 94, 97,100,102,103, 96, 99,101,103,104,105,106,107,109,111,
    106,108,110,112,114,107,110,113,115,116,109,112,115,117,118,111,114,116,
    118,119,120,121,122,124,126,121,123,125,127,129,122,125,128,130,131,124,
    127,130,132,133,126,129,131,133,134,135,136,137,139,141,136,138,140,142,
    144,137,140,143,145,146,139,142,145,147,148,141,144,146,148,149,150,151,
    152,154,156,151,153,155,157,159,152,155,158,160,161,154,157,160,162,163,
    156,159,161,163,164,165,166,167,169,171,166,168,170,172,174,167,170,173,
    175,176,169,172,175,177,178,171,174,176,178,179,180,181,182,184,186,181,
    183,185,187,189,182,185,188,190,191,184,187,190,192,193,186,189,191,193,
    194,195,196,197,199,201,196,198,200,202,204,197,200,203,205,206,199,202,
    205,207,208,201,204,206,208,209,210,211,212,214,216,211,213,215,217,219,
    212,215,218,220,221,214,217,220,222,223,216,219,221,223,224,225,226,227,
    229,231,226,228,230,232,234,227,230,233,235,236,229,232,235,237,238,231,
    234,236,238,239,240,241,242,244,246,241,243,245,247,249,242,245,248,250,
    251,244,247,250,252,253,246,249,251,253,254,255,256,257,259,261,256,258,
    260,262,264,257,260,263,265,266,259,262,265,267,268,261,264,266,268,269,
    270,271,272,274,276,271,273,275,277,279,272,275,278,280,281,274,277,280,
    282,283,276,279,281,283,284,285,286,287,289,291,286,288,290,292,294,287,
    290,293,295,296,289,292,295,297,298,291,294,296,298,299,300,301,302,304,
    306,301,303,305,307,309,302,305,308,310,311,304,307,310,312,313,306,309,
    311,313,314,315,316,317,319,321,316,318,320,322,324,317,320,323,325,326,
    319,322,325,327,328,321,324,326,328,329,330,331,332,334,336,331,333,335,
    337,339,332,335,338,340,341,334,337,340,342,343,336,339,341,343,344,345,
    346,347,349,351,346,348,350,352,354,347,350,353,355,356,349,352,355,357,
    358,351,354,356,358,359,360,361,362,364,366,361,363,365,367,369,362,365,
    368,370,371,364,367,370,372,373,366,369,371,373,374,375,376,377,379,381,
    376,378,380,382,384,377,380,383,385,386,379,382,385,387,388,381,384,386,
    388,389,390,391,392,394,396,391,393,395,397,399,392,395,398,400,401,394,
    397,400,402,403,396,399,401,403,404,405,406,407,409,411,406,408,410,412,
    414,407,410,413,415,416,409,412,415,417,418,411,414,416,418,419,420,421,
    422,424,426,421,423,425,427,429,422,425,428,430,431,424,427,430,432,433,
    426,429,431,433,434,435,436,437,439,441,436,438,440,442,444,437,440,443,
    445,446,439,442,445,447,448,441,444,446,448,449,450,451,452,454,456,451,
    453,455,457,459,452,455,458,460,461,454,457,460,462,463,456,459,461,463,
    464,465,466,467,469,471,466,468,470,472,474,467,470,473,475,476,469,472,
    475,477,478,471,474,476,478,479,480,481,482,484,486,481,483,485,487,489,
    482,485,488,490,491,484,487,490,492,493,486,489,491,493,494,495,496,497,
    499,501,496,498,500,502,504,497,500,503,505,506,499,502,505,507,508,501,
    504,506,508,509,510,511,512,514,516,511,513,515,517,519,512,515,518,520,
    521,514,517,520,522,523,516,519,521,523,524,525,526,527,529,531,526,528,
    530,532,534,527,530,533,535,536,529,532,535,537,538,531,534,536,538,539,
    540,541,542,544,546,541,543,545,547,549,542,545,548,550,551,544,547,550,
    552,553,546,549,551,553,554,555,556,557,559,561,556,558,560,562,564,557,
    560,563,565,566,559,562,565,567,568,561,564,566,568,569,570,571,572,574,
    576,571,573,575,577,579,572,575,578,580,581,574,577,580,582,583,576,579,
    581,583,584 };

const char *BLOCKSET_NAME[]    = { "FLAT" , "BASIC" , "EXTENDED" };
const char *RESOLUTION_NAME[]  = { "640x480" , "800x600" , "1024x768" , "1280x1024" , "1600x1200" };
const POINT2D RESOLUTION_VALUE[] = { {640,480} , {800,600} , {1024,768} , {1280,1024} , {1600,1200} };
const char *STYLE_NAME[] = { "Nostalgia" , "Marble", "Arcade" };
const char *SOUND_NAME[] = { "Blockout II" , "Blockout" };
const char *FRLIMITER_NAME[] = { "Off" , "50 fps", "60 fps", "75 fps", "100 fps" , "VSync" };
const float FRLIMITER_VALUE[] = { 0.0f, 50.0f, 60.0f, 75.0f, 100.0f , 0.0f };

// ------------------------------------------------

SetupManager::SetupManager() {

  // Default
  pitWidth  = 5;
  pitHeight = 5;
  pitDepth  = 12;
  blockSet  = BLOCKSET_FLAT;
  animationSpeed = 5;
  startLevel = 0;
  playSound = TRUE;
  scoreList = NULL;
  fullScreen = FALSE;
  windowSize = RES_800x600;
  transparentFace = 0;
  style = STYLE_CLASSIC;
  frLimit = 0;
  lineWidth = LINEW_MIN;
  soundType = SOUND_BLOCKOUT2;
  ZeroMemory(httpHome,sizeof(httpHome));
  ZeroMemory(httpProxy,sizeof(httpProxy));
  strcpy(httpHome,"www.blockout.net/blockout2");
  strcpy(httpProxy,"");
  httpProxyPort=0;
  useHttpProxy=FALSE;
  httpTimeout=30;
  ResetToQwerty();
  LoadHighScore();
  LoadSetup();

}

// ------------------------------------------------

void SetupManager::SetPitWidth(int width) {
  pitWidth = Saturate(width,MIN_PITWIDTH,MAX_PITWIDTH);
}

// ------------------------------------------------

int SetupManager::GetPitWidth() {
  return pitWidth;
}

// ------------------------------------------------

void SetupManager::SetPitHeight(int height) {
  pitHeight = Saturate(height,MIN_PITHEIGHT,MAX_PITHEIGHT);
}

// ------------------------------------------------

int SetupManager::GetPitHeight() {
  return pitHeight;
}

// ------------------------------------------------

void SetupManager::SetPitDepth(int depth) {
  pitDepth = Saturate(depth,MIN_PITDEPTH,MAX_PITDEPTH);
}

// ------------------------------------------------

int SetupManager::GetPitDepth() {
  return pitDepth;
}

// ------------------------------------------------

void SetupManager::SetBlockSet(int set) {
  blockSet = Saturate(set,BLOCKSET_FLAT,BLOCKSET_EXTENDED);
}

// ------------------------------------------------

int SetupManager::GetBlockSet() {
  return blockSet;
}

// ------------------------------------------------

void SetupManager::SetStartingLevel(int level) {
  startLevel = Saturate(level,0,9);
}

// ------------------------------------------------

int  SetupManager::GetStartingLevel() {
  return startLevel;
}

// ------------------------------------------------

void SetupManager::SetSound(BOOL play) {
  playSound = play;
}

// ------------------------------------------------

BOOL SetupManager::GetSound() {
  return playSound;
}

// ------------------------------------------------

void SetupManager::SetAnimationSpeed(int speed) {
  animationSpeed = Saturate(speed,ASPEED_SLOW,ASPEED_FAST);
}

// ------------------------------------------------

int SetupManager::GetAnimationSpeed() {
  return animationSpeed;
}

// ------------------------------------------------
BYTE SetupManager::GetKRx1() { return keyRx1; };
BYTE SetupManager::GetKRy1() { return keyRy1; };
BYTE SetupManager::GetKRz1() { return keyRz1; };
BYTE SetupManager::GetKRx2() { return keyRx2; };
BYTE SetupManager::GetKRy2() { return keyRy2; };
BYTE SetupManager::GetKRz2() { return keyRz2; };

void SetupManager::SetKRx1(BYTE key) { keyRx1=key; };
void SetupManager::SetKRy1(BYTE key) { keyRy1=key; };
void SetupManager::SetKRz1(BYTE key) { keyRz1=key; };
void SetupManager::SetKRx2(BYTE key) { keyRx2=key; };
void SetupManager::SetKRy2(BYTE key) { keyRy2=key; };
void SetupManager::SetKRz2(BYTE key) { keyRz2=key; };

void SetupManager::ResetToQwerty() {
  keyRx1 = 'Q';
  keyRy1 = 'W';
  keyRz1 = 'E';
  keyRx2 = 'A';
  keyRy2 = 'S';
  keyRz2 = 'D';
}

void SetupManager::ResetToAzerty() {
  keyRx1 = 'A';
  keyRy1 = 'Z';
  keyRz1 = 'E';
  keyRx2 = 'Q';
  keyRy2 = 'S';
  keyRz2 = 'D';
}

// ------------------------------------------------

void SetupManager::SetStyle(int style) {
  this->style = Saturate(style,STYLE_CLASSIC,STYLE_ARCADE);
}

int  SetupManager::GetStyle() {
  return style;
}

const char *SetupManager::GetStyleName() {
  return STYLE_NAME[style];
}

// ------------------------------------------------

void SetupManager::SetSoundType(int stype) {
  soundType = Saturate(stype,SOUND_BLOCKOUT2,SOUND_BLOCKOUT);
}

int  SetupManager::GetSoundType() {
  return soundType;
}

const char *SetupManager::GetSoundTypeName() {
  return SOUND_NAME[soundType];
}

// ------------------------------------------------


void SetupManager::SetFrLimiter(int fLimit) {
  frLimit = Saturate(fLimit,FR_NOLIMIT,FR_LIMITVSYNC);
}

int SetupManager::GetFrLimiter() {
  return frLimit;
}

float SetupManager::GetFrLimitTime() {
  if(frLimit==0)
    return 0.0f;
  else
    return 1.0f / FRLIMITER_VALUE[frLimit];
}

const char *SetupManager::GetFrLimitName() {
  return FRLIMITER_NAME[frLimit];
}

// ------------------------------------------------

float SetupManager::GetAnimationTime() {

  float min = 0.05f;
  float max = 0.15f;
  float speed = min + (max-min) * (float)(ASPEED_FAST - animationSpeed) / ((float)ASPEED_FAST);
  return speed;

}

// ------------------------------------------------

void SetupManager::SetHttpHome(char *home) {
  strncpy(httpHome,home,255);
  httpHome[255]=0;
}

// ------------------------------------------------

char *SetupManager::GetHttpHome() {
  return httpHome;
}

// ------------------------------------------------

void SetupManager::SetProxyName(char *name) {
  strncpy(httpProxy,name,255);
  httpProxy[255]=0;
}

// ------------------------------------------------

char *SetupManager::GetProxyName() {
  return httpProxy;
}

// ------------------------------------------------

void SetupManager::SetProxyPort(int port) {
  httpProxyPort = Saturate(port,0,65535);
}

// ------------------------------------------------

int SetupManager::GetProxyPort() {
  return httpProxyPort;
}

// ------------------------------------------------

void SetupManager::SetUseProxy(BOOL enable) {
  useHttpProxy = enable;
}

// ------------------------------------------------

BOOL SetupManager::GetUseProxy() {
  return useHttpProxy;
}

// ------------------------------------------------

void SetupManager::SetTimeout(int timeout) {
  httpTimeout = timeout;
}

// ------------------------------------------------

int SetupManager::GetTimeout() {
  return httpTimeout;
}

// ------------------------------------------------

char *SetupManager::GetName() {

  static char ret[32];
  strcpy(ret,"");

  // Default setup
  if( Check(5,5,12,BLOCKSET_FLAT) ) {
    return STR("[Flat Fun]");
  } else if ( Check(3,3,10,BLOCKSET_BASIC) )  {
    return STR("[3D Mania]");
  } else if ( Check(5,5,10,BLOCKSET_EXTENDED) ) {
    return STR("[Out of Control]");
  }

  // Generic name
  sprintf(ret,"[%dx%dx%d,%s]",pitWidth,pitHeight,pitDepth,GetBlockSetName());
  return ret;

}

// ------------------------------------------------

void SetupManager::SetFullScreen(BOOL isFull) {
  fullScreen = isFull;
}

// ------------------------------------------------

BOOL SetupManager::GetFullScreen() {
  return fullScreen;
}

// ------------------------------------------------

void SetupManager::SetTransparentFace(int transparent) {
  transparentFace = Saturate(transparent,FTRANS_MIN,FTRANS_MAX);
}

// ------------------------------------------------

int SetupManager::GetTransparentFace() {
  return transparentFace;
}

// ------------------------------------------------

void SetupManager::SetWindowSize(int size) {
  windowSize = Saturate(size,RES_640x480,RES_1600x1200);
}

// ------------------------------------------------

int SetupManager::GetWindowSize() {
  return windowSize;
}

// ------------------------------------------------

int SetupManager::GetWindowWidth() {
  return RESOLUTION_VALUE[windowSize].x;
}

// ------------------------------------------------

int SetupManager::GetWindowHeight() {
  return RESOLUTION_VALUE[windowSize].y;
}

// ------------------------------------------------

const char *SetupManager::GetResName(int res) {
  return RESOLUTION_NAME[res];
}

// ------------------------------------------------

const char *SetupManager::GetBlockSetName() {
  return BLOCKSET_NAME[blockSet];
}

// ------------------------------------------------

void SetupManager::SetLineWidth(int width) {
  lineWidth = Saturate(width,LINEW_MIN,LINEW_MAX);
}

int SetupManager::GetLineWidth() {
  return lineWidth;
}

float SetupManager::GetLineRadius() { 

  if( lineWidth==0 ) {
    return 0.0f;
  } else {
    return (float)lineWidth * 0.002f + 0.005f;
  }

}

// ------------------------------------------------

int SetupManager::GetId() {
  
  int idx = (blockSet-BLOCKSET_FLAT)*325 + (pitDepth-MIN_PITDEPTH)*25 
          + (pitWidth-MIN_PITWIDTH)*5 + (pitHeight-MIN_PITHEIGHT);

  return setupId[idx];

}

// ------------------------------------------------

int SetupManager::InsertHighScore(SCOREREC *score,SCOREREC **added) {

  *added = NULL;

  if( score->score == 0 ) {
    // Does not insert null score
    return 10;
  }

  int pos = 0;
  int id = GetId();
  BOOL found = FALSE;

  SCOREREC *ptr = scoreList;
  SCOREREC *lastPtr = NULL;

  // Look for insertion pos
  while( (ptr!=NULL) && (pos<10) && (!found) ) {
    found = score->score > ptr->score;
    if( !found ) {
      if( ptr->setupId == id )
        pos++;
      lastPtr = ptr;
      ptr = ptr->next;
    }
  }

  if( pos<10 ) {
    // Got a high score
    SCOREREC *nPtr = (SCOREREC *)malloc( sizeof(SCOREREC) );
    memcpy(nPtr,score,sizeof(SCOREREC));
    if( lastPtr==NULL ) {
      // New head      
      nPtr->next = scoreList;
      scoreList = nPtr;
    } else {
      // Insert after lastPtr
      nPtr->next = ptr;
      lastPtr->next = nPtr;
    }
    *added = nPtr;
    // Keep only 10
    CleanHighScore(id);
  }

  return pos;

}

// ------------------------------------------------

void SetupManager::CleanHighScore(int id) {

  int pos = 0;
  BOOL removed;
  SCOREREC *ptr = scoreList;
  SCOREREC *lastPtr = NULL;

  // Keep the 10 best score of the given setup
  while( ptr!=NULL ) {
    removed = FALSE;
    if( ptr->setupId == id ) {
      if( pos>=10 ) {
        // Remove (REM: lastPtr cannot be null)
        lastPtr->next = ptr->next;
        free(ptr);
        // Next item
        removed = TRUE;
        ptr = lastPtr->next;
      } else {
        pos++;
      }
    }
    if( !removed ) {
      lastPtr = ptr;
      ptr = ptr->next;
    }
  }

}

// ------------------------------------------------

void SetupManager::GetHighScore(SCOREREC *hScore) {

  SCOREREC *ptr = scoreList;
  int id = GetId();
  int pos = 0;

  // Return all high score of the current game setup (10 MAX)
  while( ptr!=NULL ) {
    if( ptr->setupId == id ) {
      if(pos<10) memcpy(hScore + pos,ptr,sizeof(SCOREREC));
      pos++;
    }
    ptr = ptr->next;
  }

  // Reset last items
  for(;pos<10;pos++) {
    memset(hScore + pos,0,sizeof(SCOREREC));
    strcpy( hScore[pos].name , ".........." );
  }

}

// ------------------------------------------------

int SetupManager::GetHighScore() {

  SCOREREC *ptr = scoreList;
  int id = GetId();
  BOOL found = FALSE;

  // Return the high score of the current game setup
  while(ptr!=NULL && !found) {
    found = (ptr->setupId == id);
    if( !found ) ptr = ptr->next;
  }

  if( found ) 
    return ptr->score;
  else
    return 0;

}

// ------------------------------------------------

void SetupManager::ReadScoreItem(FILE *f,SCOREREC *dest) {

  DWORD nbRead;

  int structSzie = 64;
  
  // Done for 64/32 bits compatibily 
  // 4 last bytes are not used
  nbRead=(DWORD)fread(dest, 1, structSzie, f);
  dest->next = NULL;

}

// ------------------------------------------------

void SetupManager::LoadHighScore() {

  SCOREREC *ptr;

  FILE *file = fopen(LHD(STR("hscore.dat")),"rb");

  if( file != NULL ) {

    int32 nbScore;
    DWORD nbRead;
	nbRead = (DWORD)fread(&nbScore, 1, sizeof(int32), file);

    if( (nbRead == sizeof(int32)) && (nbScore <= 10*585) && (nbScore>=1) ) {

      scoreList = (SCOREREC *)malloc( sizeof(SCOREREC) );
      ReadScoreItem(file,scoreList);
      ptr = scoreList;

      for( int i = 1; i < nbScore ; i++ ) {

        SCOREREC *nPtr = (SCOREREC *)malloc( sizeof(SCOREREC) );
        ReadScoreItem(file,nPtr);
        ptr->next = nPtr;
        ptr = ptr->next;

      }

      ptr->next = NULL;

    }

    fclose(file);

  }

}

// ------------------------------------------------

void SetupManager::WriteScoreItem(FILE *f,SCOREREC *dest) {

  int structSzie = 64;

  // Done for 64/32 bits compatibily 
  // 4 last bytes are not used
  fwrite(dest, 1, structSzie , f);

}

// ------------------------------------------------

void SetupManager::SaveHighScore() {

  SCOREREC *ptr = scoreList;

  FILE *file = fopen(LHD(STR("hscore.dat")),"wb");

  if( file != NULL ) {

    // Get number of score
    int32 nbScore = 0;
    while( ptr != NULL ) {
      nbScore++;
      ptr = ptr->next;
    }

    // Write
    DWORD nbWritten;
	nbWritten = (DWORD)fwrite(&nbScore, 1, sizeof(int32), file);
    if( nbWritten == sizeof(int32) ) {

      ptr = scoreList;
      while( ptr != NULL ) {
        WriteScoreItem(file,ptr);
        ptr = ptr->next;
      }

    }
    fclose(file);

  }

}

// ------------------------------------------------

void SetupManager::WriteSetup() {

    
  FILE *file = fopen(LHD(STR("setup.dat")),"wb");

  if( file != NULL ) {

    // Write
    DWORD nbWritten;
    int32 version = SETUP_VERSION;
	nbWritten = (DWORD)fwrite(&version, 1, sizeof(int32), file);
    if( nbWritten == sizeof(int32) ) {

      fwrite(&pitWidth,sizeof(int32),1,file);
      fwrite(&pitHeight,sizeof(int32),1,file);
      fwrite(&pitDepth,sizeof(int32),1,file);
      fwrite(&blockSet,sizeof(int32),1,file);
      fwrite(&animationSpeed,sizeof(int32),1,file);
      fwrite(&startLevel,sizeof(int32),1,file);
      fwrite(&playSound,sizeof(BOOL),1,file);
      fwrite(&fullScreen,sizeof(BOOL),1,file);
      fwrite(&windowSize,sizeof(int32),1,file);
      fwrite(&keyRx1,sizeof(BYTE),1,file);
      fwrite(&keyRy1,sizeof(BYTE),1,file);
      fwrite(&keyRz1,sizeof(BYTE),1,file);
      fwrite(&keyRx2,sizeof(BYTE),1,file);
      fwrite(&keyRy2,sizeof(BYTE),1,file);
      fwrite(&keyRz2,sizeof(BYTE),1,file);
      fwrite(&transparentFace,sizeof(int32),1,file);
      fwrite(httpHome,256,1,file);
      fwrite(httpProxy,256,1,file);
      fwrite(&httpProxyPort,sizeof(int32),1,file);
      fwrite(&useHttpProxy,sizeof(BOOL),1,file);
      fwrite(&httpTimeout,sizeof(int32),1,file);
      fwrite(&style,sizeof(int32),1,file);
      fwrite(&soundType,sizeof(int32),1,file);
      fwrite(&frLimit,sizeof(int32),1,file);
      fwrite(&lineWidth,sizeof(int32),1,file);

    }
    fclose(file);

  }

}

// ------------------------------------------------

void SetupManager::LoadSetup() {

  FILE *file = fopen(LHD(STR("setup.dat")),"rb");

  if( file != NULL ) {

    int version = 0;
    DWORD nbRead;
	nbRead = (DWORD)fread(&version, 1, sizeof(int32), file);

    if( nbRead == sizeof(int32) ) {

      if( version<=SETUP_VERSION ) {

        nbRead=(DWORD)fread(&pitWidth,sizeof(int32),1,file);
		nbRead=(DWORD)fread(&pitHeight, sizeof(int32), 1, file);
		nbRead=(DWORD)fread(&pitDepth, sizeof(int32), 1, file);
		nbRead=(DWORD)fread(&blockSet, sizeof(int32), 1, file);
		nbRead=(DWORD)fread(&animationSpeed, sizeof(int32), 1, file);
        // Convert if version 1
        if( version==1 ) {
          switch(animationSpeed) {
            case 1: // Medium (SETUP version 1)
              animationSpeed = 5;
              break;
            case 2: // FAST (SETUP version 1)
              animationSpeed = 10;
              break;
          }
        }
		nbRead=(DWORD)fread(&startLevel, sizeof(int32), 1, file);
		nbRead=(DWORD)fread(&playSound, sizeof(BOOL), 1, file);
		nbRead=(DWORD)fread(&fullScreen, sizeof(BOOL), 1, file);
		nbRead=(DWORD)fread(&windowSize, sizeof(int32), 1, file);
		nbRead=(DWORD)fread(&keyRx1, sizeof(BYTE), 1, file);
		nbRead=(DWORD)fread(&keyRy1, sizeof(BYTE), 1, file);
		nbRead=(DWORD)fread(&keyRz1, sizeof(BYTE), 1, file);
		nbRead=(DWORD)fread(&keyRx2, sizeof(BYTE), 1, file);
		nbRead=(DWORD)fread(&keyRy2, sizeof(BYTE), 1, file);
		nbRead=(DWORD)fread(&keyRz2, sizeof(BYTE), 1, file);
		nbRead=(DWORD)fread(&transparentFace, sizeof(BOOL), 1, file);
        // Convert if version < 3
        if( version<3 ) {
          if(transparentFace) {
             // Enabled -> Medium transparency
             transparentFace = 5;
          }
        }
        if( version>=3 ) {
          // Load Http proxy param
		  nbRead=(DWORD)fread(httpHome, 256, 1, file);
		  nbRead=(DWORD)fread(httpProxy, 256, 1, file);
		  nbRead=(DWORD)fread(&httpProxyPort, sizeof(int32), 1, file);
		  nbRead=(DWORD)fread(&useHttpProxy, sizeof(BOOL), 1, file);
		  nbRead=(DWORD)fread(&httpTimeout, sizeof(int32), 1, file);
        }
        if( version>=4 ) {
          // Load style and sound type
		  nbRead=(DWORD)fread(&style, sizeof(int32), 1, file);
		  nbRead=(DWORD)fread(&soundType, sizeof(int32), 1, file);
        }
        if( version>=5 ) {
          // Frame limiter
		  nbRead=(DWORD)fread(&frLimit, sizeof(int32), 1, file);
        }

        if( version>=6 ) {
          // Linewidth
		  nbRead=(DWORD)fread(&lineWidth, sizeof(int32), 1, file);
        }
      
      }

    }

    fclose(file);

  }

}

// ------------------------------------------------

BOOL SetupManager::Check(int w,int h,int d,int s) {

  return (  (w == pitWidth) && (h == pitHeight) ||
            (h == pitWidth) && (w == pitHeight) ) &&
            (d == pitDepth) && (s == blockSet);

}

// ------------------------------------------------

int SetupManager::Saturate(int v,int min,int max) {

  if( v < min ) {
    return min;
  } else if ( v > max ) {
    return max;
  }

  return v;
}
