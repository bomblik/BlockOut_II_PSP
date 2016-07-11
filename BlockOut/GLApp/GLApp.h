// -------------------------------------------
// SDL/OpenGL OpenGL application framework
// Jean-Luc PONS (2007)
// -------------------------------------------
#ifndef PLATFORM_PSP
#include <SDL.h>
#include <SDL_opengl.h>
#else
#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#endif

#include <string.h>
#include <stdlib.h>

#ifndef _GLAPPH_
#define _GLAPPH_

#define GL_OK   1
#define GL_FAIL 0

typedef int BOOL;
typedef unsigned char BYTE;

#ifndef WINDOWS
typedef unsigned int  DWORD;
#endif

#define FALSE 0
#define TRUE  1

#define DELETE_LIST(l) if(l) { glDeleteLists(l,1);l=0; }
#define DELETE_TEX(t)  if(t) { glDeleteTextures(1,&t);t=0; }

typedef struct {

  float r;
  float g;
  float b;
  float a;

} GLCOLOR;

typedef struct {

  GLCOLOR   Diffuse;
  GLCOLOR   Ambient;
  GLCOLOR   Specular;
  GLCOLOR   Emissive;
  float     Power;

} GLMATERIAL;

typedef struct {

  int x;
  int y;
  int width;
  int height;

} GLVIEWPORT;

class GLApplication {

protected:

    // Internal variables for the state of the app
    BOOL      m_bWindowed;
    char*     m_strWindowTitle;
    int       m_screenWidth;
    int       m_screenHeight;
    BOOL      m_bVSync;

    int ToggleFullscreen();

    // Variables for timing
    float             m_fTime;             // Current time in seconds
    float             m_fElapsedTime;      // Time elapsed since last frame
    float             m_fFPS;              // Instanteous frame rate
    char              m_strFrameStats[90]; // String to hold frame stats

    // Overridable variables for the app

    virtual int OneTimeSceneInit()                         { return GL_OK; }
    virtual int RestoreDeviceObjects()                     { return GL_OK; }
    virtual int FrameMove()                                { return GL_OK; }
    virtual int Render()                                   { return GL_OK; }
    virtual int InvalidateDeviceObjects()                  { return GL_OK; }
    virtual int EventProc(SDL_Event *event)                { return GL_OK; }

#ifdef PLATFORM_PSP
    SDL_Joystick* joy;
#endif

public:

    // Functions to create, run, pause, and clean up the application
	virtual int  Create(int width, int height, BOOL bFullScreen, BOOL bVSync);
    virtual void Pause(BOOL bPause);
    virtual int  Resize(DWORD width, DWORD height);
    int   Run();

    // Utils function
    static void SetMaterial(GLMATERIAL *mat);
    static void printGlError();

    // Internal constructor
    GLApplication();

private:

   int SetVideoMode();

   int m_bitsPerPixel;
   char errMsg[512];

};

#endif /* _GLAPPH_ */
