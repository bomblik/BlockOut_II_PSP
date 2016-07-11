/*
  File:        Utils.cpp
  Description: Various util functions
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
#include <math.h>
#include <time.h>
#include <stdio.h>
#undef LoadImage

#include <CImage.h>

#ifdef WINDOWS

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>

#else

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#endif

static char bl2Home[512];
static char usrHome[512];

#ifdef PLATFORM_PSP
char chracters[] = "abcdefghijklmnoprstuwz0123456789 ";
static char letter = 0;
#endif

//-----------------------------------------------------------------------------
// Name: v()
// Desc: Construct a VERTEX.
//-----------------------------------------------------------------------------
VERTEX v(float x,float y,float z) {

  static VERTEX ret;
  ret.x = x;
  ret.y = y;
  ret.z = z;
  return ret;

}

//-----------------------------------------------------------------------------
// Nomalize a 3D vector
//-----------------------------------------------------------------------------
void Normalize(VERTEX *v) {

  float n = sqrtf( v->x * v->x + v->y * v->y + v->z * v->z );
  v->x = v->x / n;
  v->y = v->y / n;
  v->z = v->z / n;

}

//-----------------------------------------------------------------------------
// Name: fround()
// Desc: Round to the nearest integer
//-----------------------------------------------------------------------------
int fround(float x) {

  int i;
  if( x<0.0f ) {
    i = (int)( (-x) + 0.5f );
    i = -i;
  } else {
    i = (int)( x + 0.5f );
  }

  return i;

}

//-----------------------------------------------------------------------------
// Name: FormatTime(float seconds)
// Desc: Format time as XXmin YYsec
//-----------------------------------------------------------------------------
char *FormatTime(float seconds) {

  static char ret[32];
  int min = (int)seconds / 60;
  int sec = (int)seconds % 60;
  sprintf(ret,"%dmin %02dsec",min,sec);

  return ret;

}

//-----------------------------------------------------------------------------
// Name: FormatDate(uint32 time)
// Desc: Format date as DD-MM-YYYY HH:MM:SS
//-----------------------------------------------------------------------------
char *FormatDate(uint32 time) {

  static char ret[32];
  if( time>0 ) {
#ifdef LOCALTIME32
    struct tm *ts = _localtime32((__time32_t *)&time);
#else
    time_t innerTm = (time_t)time;
    struct tm *ts = localtime((time_t *)&innerTm);
#endif
    sprintf(ret,"%02d-%02d-%04d %02d:%02d:%02d",ts->tm_mday,ts->tm_mon+1,ts->tm_year+1900,
                                                ts->tm_hour,ts->tm_min,ts->tm_sec);
  } else {
    strcpy(ret,"");
  }

  return ret;

}

//-----------------------------------------------------------------------------
// Name: FormatDate(uint32 time)
// Desc: Format date as DD-MM-YYYY
//-----------------------------------------------------------------------------
char *FormatDateShort(uint32 time) {

  static char ret[32];
  if( time>0 ) {
#ifdef LOCALTIME32
    struct tm *ts = _localtime32((__time32_t *)&time);
#else
    time_t innerTm = (time_t)time;
    struct tm *ts = localtime((time_t *)&innerTm);
#endif
    sprintf(ret,"%02d-%02d-%04d",ts->tm_mday,ts->tm_mon+1,ts->tm_year+1900);
  } else {
    strcpy(ret,"..........");
  }

  return ret;

}

#ifndef WINDOWS
//-----------------------------------------------------------------------------
// Name: DirExists()
// Desc: Return TRUE if the specified directory exists
//-----------------------------------------------------------------------------

BOOL DirExists(char *dirname) {

  DIR    *dp;

  if((dp  = opendir(dirname)) == NULL) {
    return FALSE;
  }
  closedir(dp);
  
  return TRUE;

}
#endif

//-----------------------------------------------------------------------------
// Name: CheckEnv()
// Desc: Check environemt
//-----------------------------------------------------------------------------
BOOL CheckEnv() {

#ifdef WINDOWS
	strcpy(usrHome,"");

	TCHAR strPath[MAX_PATH];
    if( SUCCEEDED(SHGetFolderPath( NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, strPath ) ) )
    {
      PathAppend( strPath, TEXT( "Blockout" ) );

      if( !PathFileExists( strPath ) )
      {
        if( ERROR_SUCCESS != SHCreateDirectoryEx( NULL, strPath, NULL ) ) {
		  char message[256];
              sprintf(message,"SHCreateDirectoryEx() failed\nCannot create directory\n%s",strPath);
   	      MessageBox(NULL,message,"Warning",MB_OK|MB_ICONWARNING);
          return TRUE;
		}
      }

	  strcpy(usrHome,strPath);

	} else {
          MessageBox(NULL,"SHGetFolderPath(CSIDL_LOCAL_APPDATA) failed\nCannot retreive directory","Warning",MB_OK|MB_ICONWARNING);
	}

    return TRUE;

#else

#ifndef PLATFORM_PSP
  char *homePath = getenv("HOME");
#else
  char *homePath = ".";
#endif

  if( homePath==NULL ) {
    printf("HOME environement variable if not defined !\n");
    printf("Please set the HOME variable to your home directory (ex: HOME=/users/jeanluc)\n");
    return FALSE;
  }

#ifndef PLATFORM_PSP
  char *blockoutHome = getenv("BL2_HOME");
#else
  char *blockoutHome = ".";
#endif

  if( blockoutHome==NULL ) {
    printf("BL2_HOME environement variable if not defined !\n");
    printf("Please set the BL2_HOME to the BlockOut II installation directory (ex: BL2_HOME=/usr/local/bl2).\n");
    return FALSE;
  }
  strcpy( bl2Home , blockoutHome );

#ifndef PLATFORM_PSP
  char bl2Dir[512];
  sprintf(bl2Dir,"%s/.bl2",homePath);
  if( !DirExists(bl2Dir) ) {
    // Create it
    if( mkdir(bl2Dir,S_IRWXU) < 0 ) {
      printf("Directory %s cannot be created or accessed !\n",bl2Dir);
      return FALSE;
    }
    // Set rigth
     chmod(bl2Dir,S_IRWXU);  
  }
#endif

#ifndef PLATFORM_PSP
  strcpy( usrHome , bl2Dir );
#else
  strcpy( usrHome , "./" );
#endif

  return TRUE;

#endif

}

//-----------------------------------------------------------------------------
// Name: LID()
// Desc: Locate file in the installation directory
//-----------------------------------------------------------------------------
char *LID(char *fileName) {

#ifdef WINDOWS
  return fileName;
#endif

  static char ret[512];
  sprintf(ret,"%s/%s",bl2Home,fileName);
  return ret;

}

//-----------------------------------------------------------------------------
// Name: LHD()
// Desc: Locate file in the home directory
//-----------------------------------------------------------------------------
char *LHD(char *fileName) {

  static char ret[512];

#ifdef WINDOWS
  if( strlen(usrHome)>0 ) {
    sprintf(ret,"%s\\%s",usrHome,fileName);
    return ret;
  } else {
    return fileName;
  }
#else
  sprintf(ret,"%s/%s",usrHome,fileName);
  return ret;
#endif

}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name: CreateTexture()
// Desc: Create a texture (no alpha)
//-----------------------------------------------------------------------------
int CreateTexture(int width,int height,char *imgName,GLuint *hmap) {

  *hmap = 0;

  CImage img;

  if( !img.LoadImage(LID(imgName)) ) {

#ifdef WINDOWS
    char message[256];
	sprintf(message,"CreateTexture(): %s\nFile: %s\n",img.GetErrorMessage(),imgName);
	MessageBox(NULL,message,"ERROR",MB_OK|MB_ICONERROR);
#else
    printf( "CreateTexture(): %s\nFile: %s\n",img.GetErrorMessage(),imgName );
#endif
    return GL_FAIL;

  }

  if( img.Width() != width || img.Height() != height ) {

#ifdef WINDOWS
    char message[256];
	sprintf(message,"CreateTexture(): Wrong image dimension (%dx%d required)\nFile: %s\n",width,height,LID(imgName));
	MessageBox(NULL,message,"ERROR",MB_OK|MB_ICONERROR);
#else
    printf( "CreateTexture(): Wrong image dimension (%dx%d required)\nFile: %s\n",width,height,LID(imgName) );
#endif
    return GL_FAIL;

  }

  BYTE *buff32 = (BYTE *)malloc(width*height*3);
  BYTE *data = img.GetData();
  for(int j=0;j<height;j++)
    for(int i=0;i<width;i++ ) {
      buff32[i*3+0 + j*width*3] = data[i*3+2 + j*width*3];
      buff32[i*3+1 + j*width*3] = data[i*3+1 + j*width*3];
      buff32[i*3+2 + j*width*3] = data[i*3+0 + j*width*3];
    }

  glGenTextures(1,hmap);
  glBindTexture(GL_TEXTURE_2D,*hmap);

  glTexImage2D (
    GL_TEXTURE_2D,       // Type
    0,                   // No Mipmap
    3,                  // Format RGB
    width,               // Width
    height,             // Height
    0,                   // Border
    GL_RGB,             // Format RGB
    GL_UNSIGNED_BYTE,   // 8 Bit/color
    buff32              // Data
  );   

  img.Release();
  free(buff32);

  if( glGetError() != GL_NO_ERROR ) {
#ifdef WINDOWS
    char message[256];
	sprintf(message,"CreateTexture(): OpenGL error (code=%d)\n",glGetError());
	MessageBox(NULL,message,"ERROR",MB_OK|MB_ICONERROR);
#else
    printf( "CreateTexture(): OpenGL error (code=%d)\n",glGetError() );
#endif
    return GL_FAIL;
  }

  return GL_OK;
}

#ifndef WINDOWS
//-----------------------------------------------------------------------------
// Reset memory to 0
//-----------------------------------------------------------------------------
void ZeroMemory(void *buff,int size) {
  memset(buff,0,size);
}
#endif

//-----------------------------------------------------------------------------
// Name: GetChar()
// Desc: Return char according to pressed key
//-----------------------------------------------------------------------------
extern char GetChar(BYTE *keys) {

  char retChar = 0;

  // Check letters
  for(BYTE i='A';i<='Z';i++) 
    if( keys[i] ) retChar = i;

  for(BYTE i='a';i<='z';i++) 
    if( keys[i] ) retChar = i;

  // Check numbers
  for(BYTE i=0;i<=9;i++)
    if( keys[i+'0'] ) retChar = (char)i+'0';

  // Space
  if( keys[SDLK_SPACE] ) {
    retChar = ' ';
  }

  // Dot
  if( keys['.'] ) retChar = '.';

  // Comma
  if( keys[','] ) retChar = ',';
  
  // Minus
  if( keys['-'] ) retChar = '-';

  // Plus
  if( keys['+'] ) retChar = '+';

  // Divide
  if( keys['/'] ) retChar = '/';

  // ':'
  if( keys[':'] ) retChar = ':';

  // '@'
  if( keys['@'] ) retChar = '@';

  // '''
  if( keys['\''] ) retChar = '\'';

  // '!'
  if( keys['!'] ) retChar = '!';

  // '$'
  if( keys['$'] ) retChar = '$';

  // '%'
  if( keys['%'] ) retChar = '%';

  // '&'
  if( keys['&'] ) retChar = '&';

  // '*'
  if( keys['*'] ) retChar = '*';

  // '('
  if( keys['('] ) retChar = '(';

  // ')'
  if( keys[')'] ) retChar = ')';

  // '_'
  if( keys['_'] ) retChar = '_';

  // '?'
  if( keys['?'] ) retChar = '?';

  // ';'
  if( keys[';'] ) retChar = ';';

#ifdef PLATFORM_PSP
  if ( keys[SDLK_RIGHT] ) {
    letter = 0;
  }

  if( keys[SDLK_UP] ) {
    if (letter < sizeof(chracters) - 2)
        letter++;
    else
        letter = 0;

    keys[SDLK_UP] = 0;
    retChar = chracters[letter];
  }

  if( keys[SDLK_DOWN] ) {
    if (letter >= 0)
        letter--;
    else
        letter = sizeof(chracters) - 2;

    keys[SDLK_DOWN] = 0;
    retChar = chracters[letter];
  }
#endif

  // Reset keys
  for(BYTE i='A';i<='Z';i++)
    keys[i]=0;

  for(BYTE i='a';i<='z';i++)
    keys[i]=0;

  for(BYTE i=0;i<=9;i++)
    keys[i+'0']=0;

  keys[SDLK_SPACE]=0;
  keys['.']=0;
  keys['/']=0;
  keys['-']=0;
  keys['+']=0;
  keys[':']=0;
  keys[',']=0;
  keys['.']=0;
  keys['@']=0;
  keys['\'']=0;
  keys['!']=0;
  keys['$']=0;
  keys['%']=0;
  keys['&']=0;
  keys['*']=0;
  keys['(']=0;
  keys[')']=0;
  keys['_']=0;
  keys['?']=0;
  keys[';']=0;

  return retChar;

}

