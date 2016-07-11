/*
   File:        SetupManager.h
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

#include "Types.h"

#ifndef SETUPMANAGERH
#define SETUPMANAGERH

#ifdef WINDOWS

#ifdef WIN64
#define APP_VERSION  "BlockOut 2.5 (x64)"
#else
#define APP_VERSION  "BlockOut 2.5 (x86)"
#endif

#else

#if __x86_64__ || __ppc64__
#define APP_VERSION  "BlockOut 2.5 (x64)"
#else
#define APP_VERSION  "BlockOut 2.5 (x86)"
#endif

#endif

#define SETUP_VERSION 6

class SetupManager {

  public:
    SetupManager();

    // Pit dimension
    void SetPitWidth(int width);
    int  GetPitWidth();
    void SetPitHeight(int height);
    int  GetPitHeight();
    void SetPitDepth(int depth);
    int  GetPitDepth();

    // Block set
    void SetBlockSet(int set);
    int  GetBlockSet();

    // Starting level
    void SetStartingLevel(int level);
    int  GetStartingLevel();

    // Animation speed
    void  SetAnimationSpeed(int speed);
    int   GetAnimationSpeed(); 
    float GetAnimationTime(); /* in seconds */

    // Names
    char *GetName();
    const char *GetBlockSetName();

    // Sound
    void SetSound(BOOL play);
    BOOL GetSound();

    // High score
    int  InsertHighScore(SCOREREC *score,SCOREREC **added);
    void GetHighScore(SCOREREC *hScore);
    int  GetHighScore();

    // Transparent face
    void SetTransparentFace(int transparent);
    int GetTransparentFace();

    // Return configuration id
    int GetId();

    // Save high score
    void SaveHighScore();

    // Save setup
    void WriteSetup();

    // Screen options
    void SetFullScreen(BOOL isFull);
    BOOL GetFullScreen();
    void SetWindowSize(int size);
    int  GetWindowSize();
    int  GetWindowWidth();
    int  GetWindowHeight();
    const char *GetResName(int res);

    // Game style
    void SetStyle(int style);
    int  GetStyle();
    const char *GetStyleName();

    // Game sound
    void SetSoundType(int stype);
    int  GetSoundType();
    const char *GetSoundTypeName();

    // Frame limiter
    void SetFrLimiter(int fLimit);
    int  GetFrLimiter();
    const char *GetFrLimitName();
    float GetFrLimitTime(); /* seconds */

   // Line width
   void SetLineWidth(int width);
   int GetLineWidth();
   float GetLineRadius(); /* Actual line radius */

    // Control keys
    BYTE GetKRx1();
    BYTE GetKRy1();
    BYTE GetKRz1();
    BYTE GetKRx2();
    BYTE GetKRy2();
    BYTE GetKRz2();
    void SetKRx1(BYTE key);
    void SetKRy1(BYTE key);
    void SetKRz1(BYTE key);
    void SetKRx2(BYTE key);
    void SetKRy2(BYTE key);
    void SetKRz2(BYTE key);
    void ResetToQwerty();
    void ResetToAzerty();

    // Http Config
    void SetHttpHome(char *home);
    char *GetHttpHome();
    void SetProxyName(char *name);
    char *GetProxyName();
    void SetProxyPort(int port);
    int  GetProxyPort();
    void SetUseProxy(BOOL enable);
    BOOL GetUseProxy();
    void SetTimeout(int timeout);
    int  GetTimeout();

  private:

    int Saturate(int v,int min,int max);
    BOOL Check(int w,int h,int d,int s);
    void CleanHighScore(int id);
    void LoadHighScore();
    void LoadSetup();
    void ReadScoreItem(FILE *f,SCOREREC *dest);
    void WriteScoreItem(FILE *f,SCOREREC *dest);

    int32  pitWidth;
    int32  pitHeight;
    int32  pitDepth;
    int32  blockSet;
    int32  animationSpeed;
    int32  startLevel;
    BOOL playSound;
    BOOL fullScreen;
    int32  windowSize;
    int32  transparentFace;
    int32  style;
    int32  soundType;
    int32  frLimit;
    BYTE keyRx1; // Q
    BYTE keyRy1; // W
    BYTE keyRz1; // E
    BYTE keyRx2; // A
    BYTE keyRy2; // S
    BYTE keyRz2; // D
    char httpHome[256];
    char httpProxy[256];
    int32  httpProxyPort;
    BOOL useHttpProxy;
    int32  httpTimeout;
    int32  lineWidth;

    SCOREREC *scoreList;

};

#endif /*SETUPMANAGERH*/
