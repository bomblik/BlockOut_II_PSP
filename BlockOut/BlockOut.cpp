/*
  File:        BlockOut.cpp
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

#include "BlockOut.h"

#ifndef WINDOWS
#include <unistd.h>
#else
extern void InitialiseWinsock();
#endif

#ifdef PLATFORM_PSP
#include "sys/unistd.h"
#include "GL/glu.h"

#include <pspkernel.h>
#include <psppower.h>

PSP_HEAP_SIZE_KB (16 * 1024);
PSP_MAIN_THREAD_ATTR (0);
PSP_MAIN_THREAD_STACK_SIZE_KB (4 * 1024);

enum psp_buttons {
        TRIANGLE = 0,
        CIRCLE = 1,
        CROSS = 2,
        SQUARE = 3,
        LEFT_TRIGGER = 4,
        RIGHT_TRIGGER = 5,
        DOWN = 6,
        LEFT = 7,
        UP = 8,
        RIGHT = 9,
        SELECT = 10,
        START = 11,
        HOME = 12,
        HOLD = 13,
};

#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 272

    /***************************************************************************
     * Exit Callback                                                           *
     ***************************************************************************/
int
exit_callback (int arg1, int arg2, void *common)
{
  sceKernelExitGame ();
  return 0;
}

    /***************************************************************************
     * Power Callback                                                          *
     ***************************************************************************/
int
power_callback (int unknown, int pwrflags, void *common)
{
  if (pwrflags & PSP_POWER_CB_POWER_SWITCH)
  {
  }
  else if (pwrflags & PSP_POWER_CB_RESUME_COMPLETE)
  {
  }

  return 0;
}

    /***************************************************************************
     * Callback Thread                                                         *
     ***************************************************************************/
static int
callback_thread (SceSize args, void *argp)
{
  int cbid;

  cbid = sceKernelCreateCallback ("exit callback", exit_callback, NULL);
  sceKernelRegisterExitCallback (cbid);

  cbid = sceKernelCreateCallback ("power callback", power_callback, NULL);
  scePowerRegisterCallback (0, cbid);

  sceKernelSleepThreadCB ();
  return 0;
}

    /***************************************************************************
     * Setup Callbacks                                                         *
     ***************************************************************************/
static int
setup_callbacks (void)
{
  int thid = 0;

  thid =
    sceKernelCreateThread ("update_thread", callback_thread, 0x11, 0xFA0,
               PSP_THREAD_ATTR_USER, 0);
  if (thid >= 0)
    {
      sceKernelStartThread (thid, 0, 0);
    }

  return thid;
}

extern "C" void __cxa_pure_virtual() { while(1); }
#endif

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
#ifdef WINDOWS
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
#else
#ifdef __cplusplus
    extern "C"
#endif
int main(int argc,char *argv[])
#endif
{
#ifdef PLATFORM_PSP
  setup_callbacks();
#endif

  // Check environement
  if( !CheckEnv() ) {
    return 0;
  }

#ifdef WINDOWS
  InitialiseWinsock();
#endif

  // Create and start the application
  BlockOut *glApp = new BlockOut();

#ifndef PLATFORM_PSP
  if (!glApp->Create(glApp->theSetup.GetWindowWidth(),
	  glApp->theSetup.GetWindowHeight(),
	  glApp->theSetup.GetFullScreen(),
	  glApp->theSetup.GetFrLimiter() == FR_LIMITVSYNC)) {
	  delete glApp;
	  return 0;
  }
#else
  if (!glApp->Create(SCREEN_WIDTH,
          SCREEN_HEIGHT,
          TRUE,
          FALSE)) {
          delete glApp;
          return 0;
  }
#endif

  return glApp->Run();
}

//-----------------------------------------------------------------------------
// Name: BlockOut()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
BlockOut::BlockOut()
{
  inited = FALSE;
  mode = MENU_MODE;
  m_strWindowTitle = STR(APP_VERSION);
  ZeroMemory( m_bKey, sizeof(m_bKey) );
}

//-----------------------------------------------------------------------------
// Name: UpdateFullScreen()
// Desc: Update fullscreen
//-----------------------------------------------------------------------------
int BlockOut::UpdateFullScreen()
{
  int hr = GL_OK;

  if( m_bWindowed ) {
    if( theSetup.GetFullScreen() ) {
      // Go to fullscreen
      Pause(TRUE);
      hr = ToggleFullscreen();
      Pause(FALSE);
    }
  } else {
    if( !theSetup.GetFullScreen() ) {
      // Go to windowed
      Pause(TRUE);
      hr = ToggleFullscreen();
      Pause(FALSE);
    }
  }

  return hr;

}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
int BlockOut::FrameMove()
{

  // General keys

  if( m_bKey[SDLK_F1] ) {
    BOOL fs = theSetup.GetFullScreen();
    theSetup.SetFullScreen(!fs);
    UpdateFullScreen();
    m_bKey[SDLK_F1]=0;
  }

  // Processing
  int retValue;
  switch(mode) {
    case MENU_MODE:
      retValue = theMenu.Process(m_bKey,m_fTime);
      switch( retValue ) {
        case 1: // Switch to game mode
          ZeroMemory( m_bKey, sizeof(m_bKey) );
          theGame.StartGame(m_screenWidth,m_screenHeight,m_fTime);
          mode = GAME_MODE;
          break;
        case 2: // Resize
          Resize(theSetup.GetWindowWidth() , theSetup.GetWindowHeight());
          break;
        case 3:
          UpdateFullScreen();
          break;
        case 7:
          theGame.StartDemo(m_screenWidth,m_screenHeight,m_fTime);
          mode = GAME_MODE;
          break;
        case 8:
          theGame.StartPractice(m_screenWidth,m_screenHeight,m_fTime);
          mode = GAME_MODE;
          break;
        case 100: // Exit
          InvalidateDeviceObjects();
#ifndef PLATFORM_PSP
          BOOL fs = theSetup.GetFullScreen();
          if (fs) {
            theSetup.SetFullScreen(!fs);
            UpdateFullScreen();
          }
          _exit(0);
#else
          SDL_Quit();
          sceKernelExitGame ();
          return 0;
#endif
          break;
      }
      break;
    case GAME_MODE:

      // Frame limiter
      int toSleep = (int)(theSetup.GetFrLimitTime()*1000.0f);
      if( toSleep>0 ) {
        int elapsed = SDL_GetTicks() - lastSleepTime;
        toSleep -= elapsed;
#ifdef WINDOWS
        if(toSleep>0) Sleep(toSleep);
#else
 #ifdef PLATFORM_PSP
        if(toSleep>0) SDL_Delay(toSleep*1000);
 #else
        if(toSleep>0) usleep(toSleep*1000);
 #endif
#endif
        lastSleepTime = SDL_GetTicks();
      }

      retValue = theGame.Process(m_bKey,m_fTime);

      switch( retValue ) {
        case 1: {
          // Check High Score
          SCOREREC *score = theGame.GetScore();
          SCOREREC *newScore;
          int pos = theSetup.InsertHighScore(score,&newScore);
          if( newScore ) {
            switch(theSetup.GetSoundType()) {
              case SOUND_BLOCKOUT2:
                theSound.PlayWellDone();
                break;
              case SOUND_BLOCKOUT:
                theSound.PlayWellDone2();
                break;
            }
          }
          theMenu.ToPage(&theMenu.hallOfFamePage,pos,(void *)newScore);
          // Switch to menu mode
          mode = MENU_MODE;
        }
        break;
        case 2: // Return from Demo
          ZeroMemory( m_bKey, sizeof(m_bKey) );
          mode = MENU_MODE;
          theMenu.FullRepaint();
          break;
      }
      break;
  }

  return GL_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
int BlockOut::Render()
{
    if(!inited) return GL_OK;

    // Begin the scene
    switch( mode ) {
      case MENU_MODE:
        theMenu.Render();
#ifdef _DEBUG
        // Output statistics
        m_pSmallFont.DrawText( 10,  10, m_strFrameStats );
#endif
        break;
      case GAME_MODE:
        theGame.Render();
#ifdef _DEBUG
        // Output statistics
        m_pSmallFont.DrawText( 200,  20, m_strFrameStats );
#endif
        break;
    }

    return GL_OK;
}

//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
int BlockOut::OneTimeSceneInit()
{

  if( !theSound.Create() ) {
#ifdef WINDOWS
    char message[256];
	sprintf(message,"Failed to initialize sound manager.\nNo sound will be played.\n%s\n",theSound.GetErrorMsg());
	MessageBox(NULL,message,"Warning",MB_OK|MB_ICONWARNING);
#else
    printf("Failed to initialize sound manager.\nNo sound will be played.\n%s\n",theSound.GetErrorMsg());
#endif
  }

  // Init default
  theSound.SetEnable(theSetup.GetSound());

  theHttp.SetProxy(theSetup.GetUseProxy());
  theHttp.SetProxyName(theSetup.GetProxyName());
  theHttp.SetProxyPort(theSetup.GetProxyPort());

  // Set manager
  theMenu.SetSetupManager(&theSetup);
  theMenu.SetSoundManager(&theSound);

  theMenu.SetHttp(&theHttp);

  theGame.SetSetupManager(&theSetup);
  theGame.SetSoundManager(&theSound);
  theGame.pFont = &m_pSmallFont;

  return GL_OK;
}

//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
int BlockOut::RestoreDeviceObjects()
{
    GLfloat matView[16];

    m_pSmallFont.RestoreDeviceObjects(m_screenWidth,m_screenHeight);

    //Set clear color
    glClearColor( 0, 0, 0, 0 );
 
    // Set viewport
    glViewport(0,0,m_screenWidth,m_screenHeight);

    //Compute view matrix (Right Handed)
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluLookAt(0.0,0.0,0.0, 0.0,0.0,10.0, 0.0,1.0,0.0);
    glGetFloatv( GL_MODELVIEW_MATRIX , matView );

    // Light
    glShadeModel(GL_SMOOTH);

    GLfloat global_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    GLfloat ambientLight[]  = { 0.0f, 0.0f, 0.0f, 1.0f   };
    GLfloat diffuseLight[]  = { 1.0f, 1.0f, 1.0f, 1.0f   };
    GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f   };
    GLfloat position[]      = { 15.0f, 10.0f, -10.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glEnable(GL_LIGHT0);

#ifndef PLATFORM_PSP
    // Default for texure
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
#endif

    // Default for zbuff
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    //If there was any errors
    if( glGetError() != GL_NO_ERROR )
    {
        return GL_FAIL;    
    }
    
    // Set up device objects
    if( !theMenu.Create(m_screenWidth,m_screenHeight) )
      return GL_FAIL;

    if( !theGame.Create(m_screenWidth,m_screenHeight) )
      return GL_FAIL;
  
    theGame.SetViewMatrix(matView);
    theMenu.SetViewMatrix(matView);

    inited = TRUE;
    return GL_OK;
}

//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
int BlockOut::InvalidateDeviceObjects()
{
    inited = FALSE;
    m_pSmallFont.InvalidateDeviceObjects();
    theGame.InvalidateDeviceObjects();
    theMenu.InvalidateDeviceObjects();

    return GL_OK;
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
int BlockOut::EventProc(SDL_Event *event)
{

  /*
    case WM_PAINT:
    // Need a full repaint
    theGame.FullRepaint();
    theMenu.FullRepaint();
    break;
  */

#ifndef PLATFORM_PSP
  // Handle key presses
  if( event->type == SDL_KEYDOWN )
  {
    int unicode = (event->key.keysym.unicode & 0x7F);
    if( unicode ) {
      m_bKey[unicode] = 1;
    } else {
      m_bKey[event->key.keysym.sym] = 1;
    }
  }
#else
  // Handle key presses
  if( event->type == SDL_JOYBUTTONDOWN)
  {
    switch ( event->jbutton.button )
    {
      case START:
        m_bKey[SDLK_p] = 1;
        break;
      case SELECT:
        break;
      case LEFT_TRIGGER:
        m_bKey[SDLK_z] = 1;
        break;
      case RIGHT_TRIGGER:
        m_bKey[SDLK_ESCAPE] = 1;
        break;
      case UP:
        m_bKey[SDLK_UP] = 1;
        break;
      case DOWN:
        m_bKey[SDLK_DOWN] = 1;
        break;
      case LEFT:
        m_bKey[SDLK_LEFT] = 1;
        break;
      case RIGHT:
        m_bKey[SDLK_RIGHT] = 1;
        break;
      case CROSS:
        m_bKey[SDLK_RETURN] = 1;
        m_bKey[SDLK_SPACE] = 1;
        break;
      case CIRCLE:
        if (m_bKey[SDLK_z])
          m_bKey[SDLK_d] = 1;
        else
          m_bKey[SDLK_e] = 1;
        break;
      case TRIANGLE:
        if (m_bKey[SDLK_z])
          m_bKey[SDLK_s] = 1;
        else
          m_bKey[SDLK_w] = 1;
        break;
      case SQUARE:
        if (m_bKey[SDLK_z])
          m_bKey[SDLK_a] = 1;
        else
          m_bKey[SDLK_q] = 1;
        break;
      default:
        break;
    }
  }
  else if( event->type == SDL_JOYBUTTONUP)
  {
    switch ( event->jbutton.button )
    {
      case LEFT_TRIGGER:
        m_bKey[SDLK_z] = 0;
        break;
      default:
        break;
    }
  }
#endif

  return GL_OK;

}
