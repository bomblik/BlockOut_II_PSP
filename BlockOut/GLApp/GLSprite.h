// -----------------------------------------------
// 2D sprites
// -----------------------------------------------
#ifndef PLATFORM_PSP
#include "SDL_opengl.h"
#else
#include "SDL/SDL_opengl.h"
#endif

#ifndef _SPRITE2DH_
#define _SPRITE2DH_

class Sprite2D {

public:

  // Default constructor
  Sprite2D();
  
  // Initialise the sprite
  // return 1 when success, 0 otherwise
  int RestoreDeviceObjects(char *diffName,char *alphaName,int srcWidth,int scrHeight);

  // Update sprite mapping and coordinates
  void UpdateSprite(int x1,int y1,int x2,int y2);
  void UpdateSprite(int x1,int y1,int x2,int y2,float mx1,float my1,float mx2,float my2);
  void SetSpriteMapping(float mx1,float my1,float mx2,float my2);
  
  // Draw a 2D sprite (in screen coordinates)
  void Render();

  // Release any allocated resource
  void InvalidateDeviceObjects();

private:

  GLuint  texId;
  int x1;
  int y1;
  int x2;
  int y2;
  float mx1;
  float my1;
  float mx2;
  float my2;
  int hasAlpha;

  GLfloat pMatrix[16];

};

#endif /* _SPRITE2DH_ */
