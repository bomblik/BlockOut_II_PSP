/*
   File:        Game.h
  Description: Game management
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

#include <math.h>
#include "GLApp/GLApp.h"
#include "GLApp/GLSprite.h"
#include "GLApp/GLMatrix.h"
#include "GLApp/GLFont.h"
#include "BotPlayer.h"
#include "Sprites.h"
#include "SetupManager.h"
#include "SoundManager.h"

class Game {

  public:
    Game();

    // Set manager
    void SetSetupManager(SetupManager *manager);
    void SetSoundManager(SoundManager *manager);

    // Initialise device objects
    int Create(int width,int height);

    // Render the Game
    void Render();

    // Release device objects
    void InvalidateDeviceObjects();

    // Start a game
    void StartGame(int width,int height,float fTime);

    // Start a practice
    void StartPractice(int width,int height,float fTime);

    // Start a demo
    void StartDemo(int width,int height,float fTime);

    // Process game
    int Process(BYTE *keys,float fTime);

    // Ask full repaint
    void FullRepaint();

    // Get score
    SCOREREC *GetScore();

    // Set view matrix
    void SetViewMatrix(GLfloat *mView);


    GLFont2D *pFont;

  private:

    // The pit
    Pit thePit;

    // PolyCubes 
    PolyCube allPolyCube[NB_POLYCUBE]; // All polycubes (41 items)
    int   possible[NB_POLYCUBE];       // All possible polycubes for the setup
    int   nbPossible;                  // Number of possible polycube

    // Game sprites
    Sprites sprites;

    // Manager
    SetupManager *setupManager;
    SoundManager *soundManager;

    // Game stuff
    SCOREREC score;     // Score
    int   level;        // Current level
    int   highScore;    // High score of the current game setup
    int   gameMode;     // Game mode
    int   pIdx;         // Current polycube index (in the allPolyCube array)
    int   exitValue;    // Go back to menu when 1
    int   cubePerLevel; // Number of cube per level
    int   dropPos;      // Drop position
    int   cursorPos;    // Descent cursor

    // AI stuff (Demo mode)
    BotPlayer botPlayer;       // AI player
    AI_MOVE AIMoves[MAX_MOVE]; // AI computed moves
    int     nbAIMove;          // Number of AI moves
    int     curAIMove;         // Current AI move
    float   lastAIMoveTime;    // Last move time stamp
    BOOL    demoFlag;          // Demo flag
    BOOL    practiceFlag;      // Practice flag

    // PolyCube coordinates
    GLfloat     mat[16];     // Global transform matrix
    GLMatrix    matRot;      // Current rotation matrix (frame)
    GLMatrix    newMatRot;   // Virtual rotation matrix
    VERTEX      vPos;        // Current position (frame)
    VERTEX      vTransPos;   // Virtual position
    VERTEX      vOrgPos;     // Origin of translation
    int         xPos;        // Virtual position
    int         yPos;        // Virtual position
    int         zPos;        // Virtual position

    // Time stamps
    float startTranslateTime;
    float startRotateTime;
    float startRedTime;
    float startPauseTime;
    float startStepTime;
    float startGameTime;
    float startSpark;
    float startEndTime;

    // Animation time
    float animationTime;
    float stepTime;

    // Modes
    int   rotateMode;
    BOOL  dropMode;
    BOOL  dropped;
    BOOL  lastKeyMode;
    BOOL  redMode;

    // Misc
    float curTime;
    int   fullRepaint;
    int   transparent;
    BOOL  inited;
    int   style;
    int   lineWidth;
    BOOL  endAnimStarted;

    // Constant matrix
    GLMatrix matRotOx;
    GLMatrix matRotOy;
    GLMatrix matRotOz;
    GLMatrix matRotNOx;
    GLMatrix matRotNOy;
    GLMatrix matRotNOz;

    // Viewport and transformation matrix
    GLVIEWPORT   spriteView;
    GLVIEWPORT   pitView;
    GLfloat      matProj[16];
    GLfloat      matView[16];
    GLfloat      pitMatrix[16];

    // Background
    Sprite2D           background;

    // Spark
    Sprite2D           spark;     // Spark

    // Help mode
    GLMatrix  matAI;
    float startShowAI;

    // Private methods
    void HandleKey(BYTE *keys);
    BOOL StartRotate(int rType);
    BOOL StartTranslate(int tType);
    void InitTranslate();
    void StartDrop();
    void AddPolyCube();
    void NewPolyCube(BYTE *keys);
    int  SelectPolyCube();
    void TransformCube(GLMatrix *matRot,BLOCKITEM *cubes,int nbCube,int tx,int ty,int tz);
    BOOL IsOverlap(GLMatrix *matRot,int tx,int ty,int tz,int *ox,int *oy,int *oz);
    BOOL IsOverlap(GLMatrix *matRot,int tx,int ty,int tz);
    BOOL IsOverlap(GLMatrix *matRot,int tx,int ty,int tz,BLOCKITEM *pos);
    BOOL IsOverlap(GLMatrix *matRot,int tx,int ty,int tz,int *ox,int *oy,int *oz,BLOCKITEM *pos);
    BOOL IsLower();
    int  GetBottom();
    int InitPolyCube(BOOL transparent,float wEdge);
    void ComputeScore(int nbLines,BOOL pitEmpty);
    void StartSpark(BLOCKITEM *pos);
    void RenderPracticeHelp();
    void ComputeHelp();

};
