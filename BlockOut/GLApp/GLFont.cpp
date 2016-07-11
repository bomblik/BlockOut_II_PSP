// -----------------------------------------------
// Simple 2D font
// -----------------------------------------------
#include "GLFont.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#undef LoadImage

#include <CImage.h>

extern char *LID(char *fileName);

// -------------------------------------------

GLFont2D::GLFont2D() {
}

// -------------------------------------------

int GLFont2D::RestoreDeviceObjects(int scrWidth,int scrHeight) {
  // Load the image
  CImage img;
  if( !img.LoadImage(LID((char *)"images/font.png")) ) {
#ifdef WINDOWS
    char message[256];
	sprintf(message,"Failed to load %s\n",LID((char *)"images/font.png"));
	MessageBox(NULL,message,"ERROR",MB_OK|MB_ICONERROR);
#else
    printf("Failed to load %s\n",LID((char *)"images/font.png"));
#endif
	return 0;
  }

  // Make 32 Bit RGBA buffer
  fWidth  = img.Width();
  fHeight = img.Height();
  BYTE *buff32 = (BYTE *)malloc(fWidth*fHeight*4);
  BYTE *data   = img.GetData();
  for(int y=0;y<fHeight;y++) {
    for(int x=0;x<fWidth;x++) {
      buff32[x*4 + 0 + y*4*fWidth] = data[x*3+2 + y*3*fWidth];
      buff32[x*4 + 1 + y*4*fWidth] = data[x*3+1 + y*3*fWidth];
      buff32[x*4 + 2 + y*4*fWidth] = data[x*3+0 + y*3*fWidth];
      buff32[x*4 + 3 + y*4*fWidth] = data[x*3+1 + y*3*fWidth]; // Green as alpha
    }
  }

  glGenTextures(1,&texId);
  glBindTexture(GL_TEXTURE_2D,texId);

  glTexImage2D (
    GL_TEXTURE_2D,       // Type
    0,                   // No Mipmap
    4,                   // Format RGBA
    fWidth,              // Width
    fHeight,             // Height
    0,                   // Border
    GL_RGBA,             // Format RGBA
    GL_UNSIGNED_BYTE,    // 8 Bit/color
    buff32               // Data
  );

  free(buff32);
  img.Release();

  if( glGetError() != GL_NO_ERROR )
  {
#ifdef WINDOWS
    char message[256];
	sprintf(message,"GLFont2D::RestoreDeviceObjects(): Failed to create font texture: glcode=%d\n",glGetError());
	MessageBox(NULL,message,"ERROR",MB_OK|MB_ICONERROR);
#else
    printf("GLFont2D::RestoreDeviceObjects(): Failed to create font texture: glcode=%d\n",glGetError());
#endif
    return 0;    
  }

  // Compute othographic matrix (for Transfomed Lit vertex)
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho( 0, scrWidth, scrHeight, 0, -1, 1 );
  glGetFloatv( GL_PROJECTION_MATRIX , pMatrix );

  return 1;

}

// -------------------------------------------

void GLFont2D::InvalidateDeviceObjects() {

  if(texId) glDeleteTextures(1, &texId);
  texId = 0;

}

// -------------------------------------------

void GLFont2D::DrawText(int x,int y,char *text) {

  int lgth = (int)strlen(text);

  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glColor3f(1.0f,1.0f,0.0f);
  glMatrixMode( GL_PROJECTION );
  glLoadMatrixf(pMatrix);
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  for(int i=0;i<lgth;i++ ) {

    char  c = text[i];
    float xPos = (float)((c % 16) * 16 + 1)/ (float)fWidth;
    float yPos = (float)((c / 16) * 16 )/ (float)fHeight;
    float cW   = 9.0f / (float)fWidth;
    float cH   = 15.0f / (float)fWidth;

    glBegin(GL_QUADS);
    glTexCoord2f(xPos   ,yPos   );glVertex2i(x+9*i    ,y   );
    glTexCoord2f(xPos+cW,yPos   );glVertex2i(x+9*(i+1),y   );
    glTexCoord2f(xPos+cW,yPos+cH);glVertex2i(x+9*(i+1),y+15);
    glTexCoord2f(xPos   ,yPos+cH);glVertex2i(x+9*i    ,y+15);
    glEnd();

  }

}
