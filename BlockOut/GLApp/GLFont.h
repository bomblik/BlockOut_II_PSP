// -----------------------------------------------
// Simple 2D font
// -----------------------------------------------
#ifndef PLATFORM_PSP
#include "SDL_opengl.h"
#else
#include "SDL/SDL_opengl.h"
#endif

#ifndef _GLFONT2DH_
#define _GLFONT2DH_

class GLFont2D {

public:

  // Default constructor
  GLFont2D();
  
  // Initialise the font
  // return 1 when success, 0 otherwise
  int RestoreDeviceObjects(int srcWidth,int scrHeight);
  
  // Draw a 2D text (in screen coordinates)
  void DrawText(int x,int y,char *text);

  // Release any allocated resource
  void InvalidateDeviceObjects();

private:

  GLuint  texId;
  int     fWidth;
  int     fHeight;
  GLfloat pMatrix[16];

};

#endif /* _GLFONT2DH_ */
