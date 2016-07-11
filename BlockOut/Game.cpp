/*
   File:        Game.cpp
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

#include "Game.h"
#include <time.h>
#include <ctype.h>

// Score calculation factors, used by Game::ComputeScore()
// (coming from measurements made with the original BlockOut game)

// PolyCube Level Factor (from level 0 to 10)
const float pLevelFactor[] = { 0.066990f , 0.139195f , 0.219800f , 0.308444f , 0.403897f , 
                               0.507822f , 0.619062f , 0.738630f , 0.865802f , 1.000000f ,
                               1.133333f };

// Depth Factor (from depth 6 to 18)
const float depthFactor[] = { 0.0f , 0.0f , 0.0f , 0.0f , 0.0f , 0.0f , 
                              1.557692f, 1.367521f, 1.217949f, 1.100427f, 1.000000f,
                              0.918803f, 0.852564f, 0.788996f, 0.737714f, 0.691774f, 
                              0.651709f, 0.614850f, 0.583868f };

// Line Level Factor (from level 0 to 10)
const float lLeveFactor[] = { 0.096478f, 0.163873f, 0.242913f, 0.328261f, 0.422329f,
                              0.518394f, 0.630405f, 0.747501f, 0.867087f, 1.000000f,
                              1.131653f };

// Line number factor (from 1 to 5)
const float lNumberFactor[] = { 0.0f , 1.000000f, 3.703372f, 8.104827f, 14.188325f, 22.144941f };

// Line base score (FLAT,BASIC,EXTENDED)
const float lineBase[] = { 762.5f , 875.5f , 2886.25f };

// Step Time base (seconds)
const float timeBase = 5.51f;

// Time Level factor
const float timeLevelFactor = 0.64f;

// Drop time (seconds)
const float dropTime = 0.16f;

// Spark time
const float sparkTime = 0.5f;

// ---------------------------------------------------------------------


Game::Game() {

    transparent = 0;
    style = 0;
    lineWidth = 0;
    inited = FALSE;


    // Init default rotation matrix

    matRotOx.Init33( 1.0f , 0.0f , 0.0f ,
                     0.0f , 0.0f ,-1.0f ,
                     0.0f , 1.0f , 0.0f );

    matRotOy.Init33( 0.0f , 0.0f ,-1.0f ,
                     0.0f , 1.0f , 0.0f ,
                     1.0f , 0.0f , 0.0f );

    matRotOz.Init33( 0.0f ,-1.0f , 0.0f ,
                     1.0f , 0.0f , 0.0f ,
                     0.0f , 0.0f , 1.0f );
                           
    matRotNOx.Init33( 1.0f , 0.0f , 0.0f ,
                      0.0f , 0.0f , 1.0f ,
                      0.0f ,-1.0f , 0.0f );

    matRotNOy.Init33( 0.0f , 0.0f , 1.0f ,
                      0.0f , 1.0f , 0.0f ,
                     -1.0f , 0.0f , 0.0f );

    matRotNOz.Init33( 0.0f , 1.0f , 0.0f ,
                     -1.0f , 0.0f , 0.0f ,
                      0.0f , 0.0f , 1.0f );

}

// ---------------------------------------------------------------------

void Game::SetSetupManager(SetupManager *m) {

  setupManager = m;

}

// ---------------------------------------------------------------------

void Game::SetSoundManager(SoundManager *manager) {

  soundManager = manager;

}

// ---------------------------------------------------------------------

int Game::Create(int width,int height) {

    // --------------------------------------------------------------

    // Check pit dimension
    int ok = thePit.GetWidth() * thePit.GetHeight() * thePit.GetDepth();
    if(ok==0) return GL_OK;

    // --------------------------------------------------------------

    if( !thePit.Create(width,height,style) )
      return GL_FAIL;

    // --------------------------------------------------------------

    // Compute pit viewport
    pitView.x      = fround( (float)width  * 0.0889f );
    pitView.y      = fround( (float)height * 0.0183f );
    pitView.width  = fround( (float)width  * 0.7197f );
    pitView.height = fround( (float)height * 0.9596f );
    pitView.y = height - (pitView.height + pitView.y);

    // Sprite viewport
    spriteView.x = 0;
    spriteView.y = 0;
    spriteView.width = width;
    spriteView.height = height;

    // Projection matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective(60.0,1.0,0.1,FAR_DISTANCE);
    glGetFloatv( GL_PROJECTION_MATRIX , matProj );

    // --------------------------------------------------------------
    
    // 2DSprite technique for background (resize)
    int hr;
    switch(style) {
#ifndef PLATFORM_PSP
      case STYLE_CLASSIC:
        hr = background.RestoreDeviceObjects(STR("images/background.png"),STR("none"),width,height);
        break;
      case STYLE_MARBLE:
        hr = background.RestoreDeviceObjects(STR("images/background2.png"),STR("none"),width,height);
        break;
      case STYLE_ARCADE:
        hr = background.RestoreDeviceObjects(STR("images/background3.png"),STR("none"),width,height);
        break;
#else
      case STYLE_CLASSIC:
        hr = background.RestoreDeviceObjects(STR("images.psp/background.png"),STR("none"),width,height);
        break;
      case STYLE_MARBLE:
        hr = background.RestoreDeviceObjects(STR("images.psp/background2.png"),STR("none"),width,height);
        break;
      case STYLE_ARCADE:
        hr = background.RestoreDeviceObjects(STR("images.psp/background3.png"),STR("none"),width,height);
        break;
#endif
    }
    background.UpdateSprite(0,0,width,height,0.0f,0.0f,1.0f,0.75f);

    if(!hr) return GL_FAIL;

    // --------------------------------------------------------------

    if( !sprites.Create(width,height) )
      return GL_FAIL;

    // --------------------------------------------------------------
#ifndef PLATFORM_PSP
    if( !spark.RestoreDeviceObjects(STR("images/spark.png"),STR("images/sparka.png"),width,height) )
#else
    if( !spark.RestoreDeviceObjects(STR("images.psp/spark.png"),STR("images.psp/sparka.png"),width,height) )
#endif
      return GL_FAIL;

    // --------------------------------------------------------------

    if( !InitPolyCube(transparent,setupManager->GetLineRadius()) )
      return GL_FAIL;

    // --------------------------------------------------------------

    FullRepaint();

    inited = TRUE;

    return GL_OK;

}

// ---------------------------------------------------------------------

void Game::SetViewMatrix(GLfloat *mView) {

  memcpy(matView , mView , 16*sizeof(GLfloat));

}

// ---------------------------------------------------------------------

void Game::Render() {

  if( !inited ) return;

  if( fullRepaint>0 || style==STYLE_ARCADE || gameMode==GAME_OVER ) {


    glDisable(GL_DEPTH_TEST);
    glViewport(spriteView.x,spriteView.y,spriteView.width,spriteView.height);
    background.Render();

    // Score
    sprites.RenderInfo(highScore,thePit.GetWidth(),thePit.GetHeight(),
                       thePit.GetDepth(),setupManager->GetBlockSet());

#ifndef PLATFORM_PSP
     thePit.RenderLevel();
 #else
    sprites.RenderPitLevels(thePit.GetLevel());
#endif

    sprites.RenderScore(score.score,level,score.nbCube);
    if( demoFlag )
      sprites.RenderDemo();
    if( practiceFlag )
      sprites.RenderPractice();

    fullRepaint--;

  }

  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(matProj);
  glViewport(pitView.x,pitView.y,pitView.width,pitView.height);

  // Draw pit
  if( endAnimStarted ) {
    glClear( GL_DEPTH_BUFFER_BIT );
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(pitMatrix);
    thePit.Render(TRUE,TRUE);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(matView);
  } else {
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(matView);
    thePit.Render(gameMode!=GAME_PAUSED,FALSE);
  }

  // Draw "PAUSE" or "GAME OVER"
  if( gameMode==GAME_OVER || gameMode==GAME_PAUSED ) {
    glDisable(GL_DEPTH_TEST);
    glViewport(spriteView.x,spriteView.y,spriteView.width,spriteView.height);
    sprites.RenderGameMode(gameMode);
    glViewport(pitView.x,pitView.y,pitView.width,pitView.height);
  }


  if( practiceFlag ) {

    glDisable(GL_DEPTH_TEST);
    glViewport(spriteView.x,spriteView.y,spriteView.width,spriteView.height);

#ifdef _DEBUG
    // AI debug stuff
    BLOCKITEM cubes[MAX_CUBE];
    int nbCube;
    allPolyCube[pIdx].CopyCube(cubes,&nbCube);
    TransformCube(&newMatRot,cubes,nbCube,xPos,yPos,zPos);
    pFont->DrawText( 90,  40, botPlayer.GetInfo(&thePit,cubes,nbCube) );
#endif

#ifndef PLATFORM_PSP
    RenderPracticeHelp();
#endif

    glViewport(pitView.x,pitView.y,pitView.width,pitView.height);
    if( startShowAI>0.0f ) {
      if( (curTime-startShowAI)>0.75f ) {
        startShowAI = 0.0f;
      } else {
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(matProj);
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(matAI.GetGL());
        allPolyCube[pIdx].Render(TRUE);
      }
    }

  }

  // Draw polycube
#ifdef PLATFORM_PSP
  glEnable(GL_SCISSOR_TEST);
  glScissor(pitView.x,pitView.y,pitView.width,pitView.height);
#endif
  if( gameMode!=GAME_PAUSED && gameMode!=GAME_OVER ) {
    if(lineWidth>0) glClear( GL_DEPTH_BUFFER_BIT );
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(matProj);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(mat);
    allPolyCube[pIdx].Render(redMode);
  }
#ifdef PLATFORM_PSP
  glDisable(GL_SCISSOR_TEST);
#endif

  glViewport(spriteView.x,spriteView.y,spriteView.width,spriteView.height);

  // Draw spark
  if( gameMode!=GAME_PAUSED && gameMode!=GAME_OVER && startSpark!=0.0f ) {
    glDisable(GL_DEPTH_TEST);
    spark.Render();
  }

}

// ---------------------------------------------------------------------

void Game::InvalidateDeviceObjects() {

    background.InvalidateDeviceObjects();
    spark.InvalidateDeviceObjects();
    thePit.InvalidateDeviceObjects();
    sprites.InvalidateDeviceObjects();
    for(int i=0;i<NB_POLYCUBE;i++)
      allPolyCube[i].InvalidateDeviceObjects();
    inited = FALSE;

}

// ---------------------------------------------------------------------

void Game::StartGame(int width,int height,float fTime) {

    // Recreate device objects if pit dimensions have changed
    if( setupManager->GetPitWidth()!=thePit.GetWidth()   ||
        setupManager->GetPitHeight()!=thePit.GetHeight() ||
        setupManager->GetPitDepth()!=thePit.GetDepth() ||
        setupManager->GetTransparentFace()!=transparent ||
        setupManager->GetStyle()!=style ||
        setupManager->GetLineWidth()!=lineWidth ) {

      thePit.SetDimension(setupManager->GetPitWidth(),
                          setupManager->GetPitHeight(),
                          setupManager->GetPitDepth());
      transparent = setupManager->GetTransparentFace();
      style = setupManager->GetStyle();
      lineWidth = setupManager->GetLineWidth();

      InvalidateDeviceObjects();

	  if( !Create(width,height) ) {

#ifdef WINDOWS
    	MessageBox(NULL,"Game::Create() failed, exiting.","Error",MB_OK|MB_ICONERROR);
#else
        printf("Game::Create() failed, exiting.\n");
#endif
	    exit(0);

	  }

    }

    // Initialise game variable
    animationTime = setupManager->GetAnimationTime();
    level = setupManager->GetStartingLevel();
    pIdx = 0;
    gameMode = GAME_PLAYING;
    thePit.Clear();
    stepTime = timeBase * powf(timeLevelFactor,(float)level);
    exitValue = 0;
    cubePerLevel = thePit.GetHeight() * 15 + thePit.GetWidth() * 15;
    highScore = setupManager->GetHighScore();
    startGameTime = fTime;
    curTime = fTime;
    memcpy(pitMatrix , matView , sizeof(GLfloat) * 16);
    endAnimStarted = FALSE;
    demoFlag = FALSE;
    practiceFlag = FALSE;

    // Initialise score structure
    memset(&score,0,sizeof(SCOREREC));
    score.date = (uint32)time(NULL);
    score.startLevel = level;
    score.setupId = setupManager->GetId();

    // Get all possible polycube
    nbPossible = 0;
    int minDim = thePit.GetWidth();
    if( thePit.GetHeight() < minDim ) minDim = thePit.GetHeight();
    for(int i=0;i<NB_POLYCUBE;i++) {
      if( allPolyCube[i].IsInSet(setupManager->GetBlockSet()) ) {
        if( allPolyCube[i].GetMaxDim() <= minDim ) {
          possible[nbPossible] = i;
          nbPossible++;
        }
      }
    }

    // Set a new seed for the random number generator
    srand((uint32)time(NULL));

    // Start
    FullRepaint();
    NewPolyCube(NULL);

}

// ---------------------------------------------------------------------

void Game::StartDemo(int width,int height,float fTime) {

  StartGame(width,height,fTime);
  botPlayer.Init(thePit.GetWidth(),thePit.GetHeight(),thePit.GetDepth(),setupManager->GetBlockSet());
  botPlayer.GetMoves(&thePit,&(allPolyCube[pIdx]),xPos,yPos,zPos,AIMoves,&nbAIMove);
  gameMode = GAME_DEMO;
  demoFlag = TRUE;

}

// ---------------------------------------------------------------------

void Game::StartPractice(int width,int height,float fTime) {

  StartGame(width,height,fTime);
  practiceFlag = TRUE;
  // Initialiase the bot player for the Help mode
  startShowAI = 0.0f;
  botPlayer.Init(thePit.GetWidth(),thePit.GetHeight(),thePit.GetDepth(),setupManager->GetBlockSet());

}


// ---------------------------------------------------------------------

void Game::RenderPracticeHelp() {
  
  if( curTime-startGameTime < 3.0f ) {

    char *helpStr = STR("Press [H] to see suggested position");
    int fSize = 9 * (int)strlen(helpStr);
  
    float sx = (float)pitView.x;
    float sy = (float)pitView.y;
    float w  = (float)pitView.width;
    float h  = (float)pitView.height;
  
    float x = sx + (w - (float)fSize) / 2.0f;
    float y = sy + (h - 16.0f) / 2.0f;
    pFont->DrawText( (int)x,  (int)y, helpStr);

  }

}

// ---------------------------------------------------------------------

SCOREREC *Game::GetScore() {

  return &score;

}

// ---------------------------------------------------------------------

int Game::Process(BYTE *keys,float fTime) {

  if( !inited ) return 0;

  float cSide = thePit.GetCubeSide();
  curTime = fTime;

  if( gameMode == GAME_OVER ) {

    // Porcess pit animation
    float pTime = fTime - startEndTime;
    if( pTime>0.0f ) {

      endAnimStarted = TRUE;

      float  zc = 5.0f * cSide;
      float  d  = 10.0f * cSide;
      float  x;
      float  y;
      float  z;
      VERTEX vUp;

      if( pTime<4.0f ) {

        x = 0.0f;
        y = -d * (pTime/4.0f);
        z = 0.0f;//(STARTZ) * (pTime/4.0f);

        vUp.x = 0.0f;
        vUp.y = 1.0f;
        vUp.z = -1.0f * (pTime/4.0f);
        Normalize(&vUp);

      } else {

        float a = (pTime-4.0f)/2.0f;
                
        x = d  * sinf(a);
        y = -d * cosf(a);
        z = 0.0f;

        vUp.x = -x;
        vUp.y = -y;
        vUp.z = -1.0f;
        Normalize(&vUp);

      }

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      gluLookAt(x,y,z, 0.0,0.0,zc + STARTZ, vUp.x , vUp.y , vUp.z);
      glGetFloatv( GL_MODELVIEW_MATRIX , pitMatrix );
      glPopMatrix();

    }

  }

  switch( gameMode ) {

    case GAME_PLAYING:
    case GAME_PAUSED:
    case GAME_OVER:
      HandleKey(keys);
      break;

    case GAME_DEMO: {

      // Abort the demo
      if( keys[SDLK_ESCAPE] ) {
        score.gameTime = curTime - startGameTime;
        gameMode = GAME_OVER;
        keys[SDLK_ESCAPE]=0;
        return 2;
      }

      // Wait end of move
      float aTime = curTime - lastAIMoveTime;
      if( aTime>animationTime && rotateMode==0 ) {

        // Execute move

        if( curAIMove>=nbAIMove ) {

          // Drop
          StartDrop();

        } else {

          startRotateTime = curTime;
          rotateMode = AIMoves[curAIMove].rotate;
          switch( rotateMode ) {
           case 1:
             newMatRot.Multiply(&matRot,&matRotOx);
             break;
           case 2:
             newMatRot.Multiply(&matRot,&matRotOy);
             break;
           case 3:
             newMatRot.Multiply(&matRot,&matRotOz);
             break;
           case 4:
             newMatRot.Multiply(&matRot,&matRotNOx);
             break;
           case 5:
             newMatRot.Multiply(&matRot,&matRotNOy);
             break;
           case 6:
             newMatRot.Multiply(&matRot,&matRotNOz);
             break;
          }

          if( AIMoves[curAIMove].tx || AIMoves[curAIMove].ty || AIMoves[curAIMove].tz ) {
            xPos += AIMoves[curAIMove].tx;
            yPos += AIMoves[curAIMove].ty;
            zPos += AIMoves[curAIMove].tz;
            InitTranslate();
          }

          curAIMove++;

        }

        lastAIMoveTime = curTime;

      }
      } break;



  }
  
  if( !(gameMode==GAME_PLAYING || gameMode==GAME_DEMO) )
    return exitValue;

  // Compute rotation
  GLMatrix tmpRotation;
  tmpRotation.Identity();

  if( rotateMode ) {
    float rTime = (fTime - startRotateTime);
    if( rTime > animationTime ) {
      // End of rotation
      matRot = newMatRot;
      rotateMode = 0;
    } else {

      float angle;
      if( rotateMode>0 ) {
        angle = PI/2.0f * (rTime/animationTime);
      } else {
        angle = PI/5.0f * sinf( (rTime/animationTime) * PI );
      }

      // Transition
      switch(rotateMode) {
        case 1:
        case -1:
          tmpRotation.RotateX(angle);
          break;
        case 2:
        case -2:
          tmpRotation.RotateY(-angle);
          break;
        case 3:
        case -3:
          tmpRotation.RotateZ(angle);
          break;
        case 4:
        case -4:
          tmpRotation.RotateX(-angle);
          break;
        case 5:
        case -5:
          tmpRotation.RotateY(angle);
          break;
        case 6:
        case -6:
          tmpRotation.RotateZ(-angle);
          break;
      }
    }
  }

  // Compute translation
  float tTime = (fTime - startTranslateTime);

  if( tTime > animationTime ) {
    // End of translation
    vPos.x = (float)xPos;
    vPos.y = (float)yPos;
    vPos.z = (float)zPos;
  } else {
    // Transition
    vPos.x = vOrgPos.x + (vTransPos.x * (tTime/animationTime));
    vPos.y = vOrgPos.y + (vTransPos.y * (tTime/animationTime));
    vPos.z = vOrgPos.z + (vTransPos.z * (tTime/animationTime));
  }

  // Compute drop
  if( dropMode ) {
    float dTime = (fTime - startTranslateTime);
    startStepTime = fTime; // No step while drop
    if( dTime > dropTime ) {
      // End of drop
      vPos.z = (float)zPos;
      dropMode = FALSE;
      // Process last key
      if( !lastKeyMode ) {
        redMode = TRUE;
        startRedTime = fTime;
      } else {
        // Fast step on drop (100 ms)
        startStepTime = fTime - stepTime + 0.1f;
        lastKeyMode = FALSE;
      }
    } else {
      vPos.x = vOrgPos.x + (vTransPos.x * (dTime/dropTime));
      vPos.y = vOrgPos.y + (vTransPos.y * (dTime/dropTime));
      vPos.z = vOrgPos.z + (vTransPos.z * (dTime/dropTime));
    }
  }

  // Step
  if( !practiceFlag && !redMode && (gameMode==GAME_PLAYING) ) {
    if( startStepTime==0.0f ) {
      // First step
      startStepTime = fTime;
    } else {
      float sTime = (fTime - startStepTime);
      if( sTime > stepTime ) {
        startStepTime = fTime;
        cursorPos++;
        dropPos--;
        if( IsLower() ) {
          if( !IsOverlap(&newMatRot,xPos,yPos,zPos+1) ) {
            zPos++;
            InitTranslate();
          } else {
            redMode = TRUE;
            startRedTime = fTime;
          }
        }
      }
    }
  }

  // Spark
  if( startSpark != 0.0f ) {

    float sTime = (fTime - startSpark); 
    if( sTime >= sparkTime ) {
      startSpark = 0.0f;
    } else {

      int frame = (int)( (sTime*16.0f)/sparkTime );
      if( frame>15 ) frame = 15;
      float x1 = (float)(frame % 4)/4.0f;
      float y1 = (float)(frame / 4)/4.0f;
      float x2 = x1 + 0.25f;
      float y2 = y1 + 0.25f;
      spark.SetSpriteMapping(x1,y1,x2,y2);

    }

  }
  
  // Red mode (end of move)
  if( redMode ) {
    float rTime = (fTime - startRedTime);
    if( rTime > 0.05f ) {
      AddPolyCube();
      NewPolyCube(keys);
    }
  }


  if( gameMode==GAME_OVER ) {
    // Start pit animation
    startEndTime = fTime + 0.5f;
  }

  // Compute block transformation matrix
  VERTEX center = allPolyCube[pIdx].GetRCenter();
  glMatrixMode( GL_MODELVIEW );
  glLoadMatrixf(matView);
  glTranslatef(vPos.x*cSide,vPos.y*cSide,vPos.z*cSide);
  glTranslatef(center.x,center.y,center.z);
  glMultMatrixf(tmpRotation.GetGL());
  glMultMatrixf(matRot.GetGL());
  glTranslatef(-center.x,-center.y,-center.z);
  glGetFloatv( GL_MODELVIEW_MATRIX , mat );

  return exitValue;
}

// ---------------------------------------------------------------------

void Game::AddPolyCube() {

   // Add the polycube to the pit
   BLOCKITEM cubes[MAX_CUBE];
   int nbCube;
   allPolyCube[pIdx].CopyCube(cubes,&nbCube);
   TransformCube(&newMatRot,cubes,nbCube,xPos,yPos,zPos);

   for(int i=0;i<nbCube;i++)
     thePit.AddCube( cubes[i].x , cubes[i].y , cubes[i].z );

   // Remove lines
   int nbLines = thePit.RemoveLines();
   BOOL pitEmpty = thePit.IsEmpty();
   if( nbLines > 0 ) {
     switch( setupManager->GetSoundType() ) {
       case SOUND_BLOCKOUT2:
         if( pitEmpty ) soundManager->PlayEmpty();
         else           soundManager->PlayLine();
         break;
       case SOUND_BLOCKOUT:
         if( pitEmpty ) soundManager->PlayEmpty2();
         else           soundManager->PlayLine2();
         break;
     }
   }

   // Upate score and level
   ComputeScore(nbLines,pitEmpty);
   if( level<10 ) {
     if( score.nbCube >= cubePerLevel * (level+1) ) {
       level++;
       stepTime *= timeLevelFactor;
       switch( setupManager->GetSoundType() ) {
         case SOUND_BLOCKOUT2:
           soundManager->PlayLevel();
           break;
         case SOUND_BLOCKOUT:
           soundManager->PlayLevel2();
           break;
       }
     }
   }

   FullRepaint();

}

// ---------------------------------------------------------------------

void Game::ComputeScore(int nbLines,BOOL pitEmpty) {

  if( !dropped )  dropPos = 0;
  if( dropPos<0 ) dropPos = 0;

  // PolyCube score
  float fPos   = (float)dropPos / (float)(thePit.GetDepth()-1);
  float lScore = (float)allPolyCube[pIdx].GetLowScore();
  float hScore = (float)allPolyCube[pIdx].GetHighScore();
  float pScore = (lScore + (hScore-lScore) * fPos) * pLevelFactor[level];

  // Line score
  float lineScore = lineBase[ setupManager->GetBlockSet() ] * lLeveFactor[level] * lNumberFactor[nbLines];

  // Empty pit ( 2 lines bonus )
  float pitScore = 0.0f;
  if( pitEmpty )
    pitScore = lineBase[ setupManager->GetBlockSet() ] * lLeveFactor[level] * lNumberFactor[2];

  // Total score
  float totalScore = ( lineScore + pScore + pitScore ) * depthFactor[ thePit.GetDepth() ];

  // Round
  int iScore = fround( totalScore );
  if( iScore<1 ) iScore = 1;

  // Update score
  score.score  += iScore;

  // Update statistics
  score.nbCube += allPolyCube[pIdx].GetNbCube();
  if(pitEmpty) score.emptyPit++;

  // Update nbLines
  switch( nbLines ) {
    case 1:
      score.nbLine1++;
      break;
    case 2:
      score.nbLine2++;
      break;
    case 3:
      score.nbLine3++;
      break;
    case 4:
      score.nbLine4++;
      break;
    case 5:
      score.nbLine5++;
      break;
  }

}

// ---------------------------------------------------------------------

void Game::NewPolyCube(BYTE *keys) {

   // Init position and rotation
   vPos = v( 0.0f , 0.0f , 0.0f );
   xPos = 0;
   yPos = 0;
   zPos = 0;
   matRot.Identity();
   newMatRot.Identity();

   // Reset keys and mode
   if( keys )
     ZeroMemory( keys, SDLK_LAST * sizeof(BYTE) );
   startRotateTime = 0.0f;
   startTranslateTime = 0.0f;
   rotateMode = 0;
   dropMode = FALSE;
   lastKeyMode = FALSE;
   redMode = FALSE;
   startStepTime = 0.0f;
   dropPos = thePit.GetDepth() - 1;
   cursorPos = 0;
   dropped = FALSE;
   lastAIMoveTime = curTime;
   curAIMove = 0;

   // Select new polycube
   pIdx = SelectPolyCube();
   if( gameMode==GAME_DEMO ) 
     botPlayer.GetMoves(&thePit,&(allPolyCube[pIdx]),xPos,yPos,zPos,AIMoves,&nbAIMove);

}

// ---------------------------------------------------------------------

void Game::StartDrop() {

  if( gameMode==GAME_PAUSED || gameMode==GAME_OVER ) return;

  if( !dropMode && !redMode && !lastKeyMode ) {

     dropMode = TRUE;
     dropped = TRUE;

     // Compute translation
     int z = zPos;
     while( !IsOverlap(&newMatRot,xPos,yPos,z+1) ) z++;
     zPos = z;
     // Position the cursor to the bottom of the block
     cursorPos = GetBottom();
     InitTranslate();

  }

}

// ---------------------------------------------------------------------

void Game::InitTranslate() {

     startTranslateTime = curTime;
     vOrgPos = vPos;
     vTransPos = v( (float)xPos - vPos.x , 
                    (float)yPos - vPos.y ,
                    (float)zPos - vPos.z );

}

// ---------------------------------------------------------------------

BOOL Game::StartTranslate(int tType) {

  if( gameMode==GAME_PAUSED || gameMode==GAME_OVER ) return TRUE;

  if( !dropMode && !redMode ) {

     switch(tType) {
        case 1: // Left
          if( !IsOverlap(&newMatRot,xPos+1,yPos,zPos) ) {
            xPos++;
            InitTranslate();
          }
          break;
        case 2: // Rigth
          if( !IsOverlap(&newMatRot,xPos-1,yPos,zPos) ) {
            xPos--;
            InitTranslate();
          }
          break;
        case 3: // Up
          if( !IsOverlap(&newMatRot,xPos,yPos+1,zPos) ) {
            yPos++;
            InitTranslate();
          }
          break;
        case 4: // Down
          if( !IsOverlap(&newMatRot,xPos,yPos-1,zPos) ) {
            yPos--;
            InitTranslate();
          }
          break;
        case 5: // Left,Up
          if( !IsOverlap(&newMatRot,xPos+1,yPos+1,zPos) ) {
            yPos++;xPos++;
            InitTranslate();
          } else if( !IsOverlap(&newMatRot,xPos+1,yPos,zPos) ) {
            xPos++;
            InitTranslate();
          } else if( !IsOverlap(&newMatRot,xPos,yPos+1,zPos) ) {
            yPos++;
            InitTranslate();
          }
          break;
        case 6: // Right,Up
          if( !IsOverlap(&newMatRot,xPos-1,yPos+1,zPos) ) {
            yPos++;xPos--;
            InitTranslate();
          } else if( !IsOverlap(&newMatRot,xPos-1,yPos,zPos) ) {
            xPos--;
            InitTranslate();
          } else if( !IsOverlap(&newMatRot,xPos,yPos+1,zPos) ) {
            yPos++;
            InitTranslate();
          }
          break;
        case 7: // Left,Down
          if( !IsOverlap(&newMatRot,xPos+1,yPos-1,zPos) ) {
            yPos--;xPos++;
            InitTranslate();
          } else if( !IsOverlap(&newMatRot,xPos+1,yPos,zPos) ) {
            xPos++;
            InitTranslate();
          } else if( !IsOverlap(&newMatRot,xPos,yPos-1,zPos) ) {
            yPos--;
            InitTranslate();
          }
          break;
        case 8: // Right,Down
          if( !IsOverlap(&newMatRot,xPos-1,yPos-1,zPos) ) {
            yPos--;xPos--;
            InitTranslate();
          } else if( !IsOverlap(&newMatRot,xPos-1,yPos,zPos) ) {
            xPos--;
            InitTranslate();
          } else if( !IsOverlap(&newMatRot,xPos,yPos-1,zPos) ) {
            yPos--;
            InitTranslate();
          }
          break;

     }

     return TRUE;

  } else {

     if( dropMode )
       lastKeyMode = TRUE;
     return FALSE;

  }

}

// ---------------------------------------------------------------------
void Game::StartSpark(BLOCKITEM *pos) {

  soundManager->PlayHit();    
  startSpark = curTime;

  // Compute spark location
  VERTEX org  = thePit.GetOrigin();
  float cSide = thePit.GetCubeSide();
  GLMatrix mProj; mProj.FromGL(matProj);
  GLMatrix mView; mView.FromGL(matView);
  GLMatrix m; m.Multiply(&mProj,&mView);
  float rx,ry,rz,rw;
  m.TransfomVec((float)((pos->x) + 0.5f) * cSide + org.x,
                (float)((pos->y) + 0.5f) * cSide + org.y,
                (float)((pos->z) + 0.5f) * cSide + org.z,
                1.0f , &rx,&ry,&rz,&rw);

  int x = (int)(((-rx / rw) + 1.0f) * (float)pitView.width/2.0f)  + pitView.x;
  int y = (int)(((ry / rw) + 1.0f) * (float)pitView.height/2.0f) + pitView.y;
  float pz = (float)pos->z;
  float pd = (float)thePit.GetDepth();
  float sw = (float)spriteView.width;
  int w = fround( sw*0.05f * ((1.0f - pz/pd)*0.33f + 0.66f) );
  spark.UpdateSprite( x-w , y-w , x+w , y+w );

}

// ---------------------------------------------------------------------

BOOL Game::StartRotate(int rType) {

  if( gameMode==GAME_PAUSED || gameMode==GAME_OVER ) return TRUE;

  if( (rotateMode<=0) && !dropMode && !redMode ) {

    rotateMode = rType;
    startRotateTime = curTime;

    // Compute new rotation matrix

    switch(rotateMode) {
     case 1:
       newMatRot.Multiply(&matRot,&matRotOx);
       break;
     case 2:
       newMatRot.Multiply(&matRot,&matRotOy);
       break;
     case 3:
       newMatRot.Multiply(&matRot,&matRotOz);
       break;
     case 4:
       newMatRot.Multiply(&matRot,&matRotNOx);
       break;
     case 5:
       newMatRot.Multiply(&matRot,&matRotNOy);
       break;
     case 6:
       newMatRot.Multiply(&matRot,&matRotNOz);
       break;
    }

    // Check overlap
    int tx,ty,tz;
    BLOCKITEM pos;
    if( IsOverlap(&newMatRot,xPos,yPos,zPos,&tx,&ty,&tz,&pos) ) {
      // Check if the rotation can be done by a combination with a 
      // translation

      if( IsOverlap(&newMatRot,xPos+tx,yPos+ty,zPos+tz) ) {
        // Abort rotation
        rotateMode = -rotateMode;
        newMatRot = matRot;
        StartSpark(&pos);
      } else {
        // Combine rotation and translation
        xPos += tx;
        yPos += ty;
        zPos += tz;
        InitTranslate();
      }
    }

    return TRUE;

  } else {

    if( dropMode )
      lastKeyMode = TRUE;
    return FALSE;

  }

}

// ---------------------------------------------------------------------

int  Game::GetBottom() {

  int max = 0;

  // Get polycube cubes and transform them
  BLOCKITEM cubes[MAX_CUBE];
  int nbCube;
  allPolyCube[pIdx].CopyCube(cubes,&nbCube);
  TransformCube(&newMatRot,cubes,nbCube,xPos,yPos,zPos);

  // Get the bottom pos
  for(int i=0;i<nbCube;i++) 
    if(cubes[i].z >= max) max=cubes[i].z;

  return max;

}

// ---------------------------------------------------------------------

BOOL Game::IsLower() {

    BOOL upper = FALSE;

    // Get polycube cubes and transform them
    BLOCKITEM cubes[MAX_CUBE];
    int nbCube;
    allPolyCube[pIdx].CopyCube(cubes,&nbCube);
    TransformCube(&newMatRot,cubes,nbCube,xPos,yPos,zPos);

    // Check if all polycubes are above the cursor
    for(int i=0;i<nbCube && !upper;i++) {
      upper = (cubes[i].z >= cursorPos);
      if(!upper) i++;
    }

    return !upper;

}

// ---------------------------------------------------------------------

BOOL Game::IsOverlap(GLMatrix *matRot,int tx,int ty,int tz) {

  return IsOverlap(matRot,tx,ty,tz,NULL,NULL,NULL,NULL);

}

// ---------------------------------------------------------------------

BOOL Game::IsOverlap(GLMatrix *matRot,int tx,int ty,int tz,int *ox,int *oy,int *oz) {

  return IsOverlap(matRot,tx,ty,tz,ox,oy,oz,NULL);

}

// ---------------------------------------------------------------------

BOOL Game::IsOverlap(GLMatrix *matRot,int tx,int ty,int tz,BLOCKITEM *pos) {

  return IsOverlap(matRot,tx,ty,tz,NULL,NULL,NULL,pos);

}

// ---------------------------------------------------------------------

BOOL Game::IsOverlap(GLMatrix *matRot,int tx,int ty,int tz,int *ox,int *oy,int *oz,BLOCKITEM *pos) {

    BOOL Overlap = FALSE;
    int mox = 0;
    int moy = 0;
    int moz = 0;
    int lox = 0;
    int loy = 0;
    int loz = 0;
    if( pos ) memset(pos,0,sizeof(BLOCKITEM));
    
    // Get polycube cubes and transform them
    BLOCKITEM cubes[MAX_CUBE];
    int nbCube;
    allPolyCube[pIdx].CopyCube(cubes,&nbCube);
    TransformCube(matRot,cubes,nbCube,tx,ty,tz);

    // Check overlap between the polycube and the pit
    for(int i=0;i<nbCube;i++) {
      if( thePit.GetValue( cubes[i].x , cubes[i].y , cubes[i].z ) ) {
        if( pos ) {
          pos->x = cubes[i].x;
          pos->y = cubes[i].y;
          pos->z = cubes[i].z;
        }
        Overlap = TRUE;
      }
      thePit.GetOutOfBounds( cubes[i].x , cubes[i].y , cubes[i].z , &lox , &loy , &loz );
      if( abs(lox) > abs(mox) ) mox = lox;
      if( abs(loy) > abs(moy) ) moy = loy;
      if( abs(loz) > abs(moz) ) moz = loz;
    }

    if( ox ) *ox = mox;
    if( oy ) *oy = moy;
    if( oz ) *oz = moz;

    return Overlap;

}

//-----------------------------------------------------------------------------

void Game::TransformCube(GLMatrix *matRot,BLOCKITEM *cubes,int nbCube,
                             int tx,int ty,int tz) {

   BLOCKITEM center = allPolyCube[pIdx].GetICenter();
   for(int i=0;i<nbCube;i++) {

     float rx,ry,rz,rw;
     matRot->TransfomVec( (float)(cubes[i].x - center.x) + 0.5f,
                          (float)(cubes[i].y - center.y) + 0.5f,
                          (float)(cubes[i].z - center.z) + 0.5f,
                          1.0f, &rx,&ry,&rz,&rw);

     cubes[i].x = fround( rx - 0.5f ) + center.x + tx ;
     cubes[i].y = fround( ry - 0.5f ) + center.y + ty ;
     cubes[i].z = fround( rz - 0.5f ) + center.z + tz ;

   }

}

//-----------------------------------------------------------------------------

int Game::SelectPolyCube() {

  // Get a new polycube
  pIdx = possible[rand() % nbPossible];
  
  // Translate the polycube to the left
  if( allPolyCube[pIdx].GetNbCube()>0 ) {
    xPos += thePit.GetWidth() - allPolyCube[pIdx].GetWidth();
    vPos.x = (float)xPos;
  }

  // Check game over state
  if( IsOverlap(&newMatRot,xPos,yPos,zPos) ) {
    score.gameTime = curTime - startGameTime;
    gameMode = GAME_OVER;
    FullRepaint();
  }

  return pIdx;

}

//-----------------------------------------------------------------------------

void Game::ComputeHelp() {

  int tx,ty,tz;

  tx = thePit.GetWidth() - allPolyCube[pIdx].GetWidth();
  ty = 0;
  tz = 0;
  botPlayer.GetMoves(&thePit,&(allPolyCube[pIdx]),tx,ty,tz,AIMoves,&nbAIMove);
  
  GLMatrix aiRot;

  for(int i=nbAIMove-1;i>=0;i--) {

    switch( AIMoves[i].rotate ) {
       case 1:
         aiRot.Multiply(&matRotOx);
         break;
       case 2:
         aiRot.Multiply(&matRotOy);
         break;
       case 3:
         aiRot.Multiply(&matRotOz);
         break;
       case 4:
         aiRot.Multiply(&matRotNOx);
         break;
       case 5:
         aiRot.Multiply(&matRotNOy);
         break;
       case 6:
         aiRot.Multiply(&matRotNOz);
         break;
    }
    tx += AIMoves[i].tx;
    ty += AIMoves[i].ty;
    tz += AIMoves[i].tz;

  }

  // Drop
  while( !IsOverlap(&aiRot,tx,ty,tz+1) ) tz++;

  // Transform
  GLMatrix matT1;
  GLMatrix matT2;
  GLMatrix matT3;
  float cSide = thePit.GetCubeSide();
  VERTEX center = allPolyCube[pIdx].GetRCenter();

  matT1.Translate(-center.x,-center.y,-center.z);
  matT2.Translate(center.x,center.y,center.z);
  matT3.Translate(tx*cSide,ty*cSide,tz*cSide);

  matAI.FromGL(matView);
  matAI.Multiply(&matT3);
  matAI.Multiply(&matT2);
  matAI.Multiply(&aiRot);
  matAI.Multiply(&matT1);

  startShowAI = curTime;

}

//-----------------------------------------------------------------------------

void Game::HandleKey(BYTE *keys) {

  if( keys['P'] || keys['p'] ) {
    if( gameMode==GAME_PLAYING ) {
      gameMode = GAME_PAUSED;
      startPauseTime = curTime;
      FullRepaint();
    } else if ( gameMode==GAME_PAUSED ) {
      gameMode = GAME_PLAYING;
      // Update time stamps
      startRotateTime    += (curTime - startPauseTime);
      startTranslateTime += (curTime - startPauseTime);
      startRedTime       += (curTime - startPauseTime);
      startStepTime      += (curTime - startPauseTime);
      startGameTime      += (curTime - startPauseTime);
      FullRepaint();
    }
    keys['P']=0;
    keys['p']=0;
  }

  if( keys[SDLK_RETURN] ) {
    if( gameMode==GAME_OVER ) {
      // Return to menu
      if( demoFlag || practiceFlag )
        exitValue = 2;
      else
        exitValue = 1;
    }
    keys[SDLK_RETURN]=0;
  }

  if( keys[SDLK_ESCAPE] ) {
    // Abort the Game and return to menu
    if( gameMode==GAME_PLAYING ) {
      score.gameTime = curTime - startGameTime;      
      gameMode=GAME_OVER;
    }
    if( demoFlag || practiceFlag )
      exitValue = 2;
    else
      exitValue = 1;
    keys[SDLK_ESCAPE]=0;
  }

  if( keys[SDLK_UP] || keys['8'] ) {
    if( StartTranslate(3) ) {
      keys[SDLK_UP]=0;
      keys['8']=0;
    }
  }

  if( keys[SDLK_DOWN] || keys['2'] ) {
    if( StartTranslate(4) ) {
      keys[SDLK_DOWN]=0;
      keys['2']=0;
    }
  }

  if( keys[SDLK_LEFT] || keys['4'] ) {
    if( StartTranslate(1) ) {
      keys[SDLK_LEFT]=0;
      keys['4']=0;
    }
  }

  if( keys[SDLK_RIGHT] || keys['6'] ) {
    if( StartTranslate(2) ) {
      keys[SDLK_RIGHT]=0;
      keys['6']=0;
    }
  }

  if( keys[SDLK_HOME] || keys['7'] ) {
    if( StartTranslate(5) ) {
      keys[SDLK_HOME]=0;
      keys['7']=0;
    }
  }

  if( keys[SDLK_PAGEUP] || keys['9'] ) {
    if( StartTranslate(6) ) {
      keys[SDLK_PAGEUP]=0;
      keys['9']=0;
    }
  }

  if( keys[SDLK_END] || keys['1'] ) {
    if( StartTranslate(7) ) {
      keys[SDLK_END]=0;
      keys['1']=0;
    }
  }

  if( keys[SDLK_PAGEDOWN] || keys['3'] ) {
    if( StartTranslate(8) ) {
      keys[SDLK_PAGEDOWN]=0;
      keys['3']=0;
    }
  }

  if( keys[SDLK_SPACE] ) {
    StartDrop();
    keys[SDLK_SPACE]=0;
  }

  if(keys[setupManager->GetKRz2()] || keys[tolower(setupManager->GetKRz2())]) {
    if( StartRotate(3) ) { // Oz
      keys[setupManager->GetKRz2()]=0;
      keys[tolower(setupManager->GetKRz2())]=0;
    }
  }

  if(keys[setupManager->GetKRy2()] || keys[tolower(setupManager->GetKRy2())]) {
    if( StartRotate(2) ) { // Oy
      keys[setupManager->GetKRy2()]=0;
      keys[tolower(setupManager->GetKRy2())]=0;
    }
  }

  if(keys[setupManager->GetKRx2()] || keys[tolower(setupManager->GetKRx2())]) {
    if( StartRotate(1) ) { // Ox
      keys[setupManager->GetKRx2()]=0;
      keys[tolower(setupManager->GetKRx2())]=0;
    }
  }

  if(keys[setupManager->GetKRz1()] || keys[tolower(setupManager->GetKRz1())]) {
    if( StartRotate(6) ) { // -Oz
      keys[setupManager->GetKRz1()]=0;
      keys[tolower(setupManager->GetKRz1())]=0;
    }
  }

  if(keys[setupManager->GetKRy1()] || keys[tolower(setupManager->GetKRy1())]) {
    if( StartRotate(5) ) { // -Oy
      keys[setupManager->GetKRy1()]=0;
      keys[tolower(setupManager->GetKRy1())]=0;
    }
  }

  if(keys[setupManager->GetKRx1()] || keys[tolower(setupManager->GetKRx1())]) {
    if( StartRotate(4) ) { // -Ox
      keys[setupManager->GetKRx1()]=0;
      keys[tolower(setupManager->GetKRx1())]=0;
    }
  }
  
  // Help mode
  if( (keys['H'] || keys['h']) && practiceFlag) {
    ComputeHelp();
    keys['H']=0;
    keys['h']=0;
  }

}

//-----------------------------------------------------------------------------

void Game::FullRepaint() {
  // 2 times for the 2 buffers
  fullRepaint = 2;
}
