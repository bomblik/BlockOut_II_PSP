/*
   File:        EditControl.cpp
  Description: Edit control (in menu page)
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

#include "Menu.h"

// ------------------------------------------------

EditControl::EditControl() {
}

// ------------------------------------------------

void EditControl::SetDisplayLength(int length) {
  displayLgth = length;
}

// ------------------------------------------------

void EditControl::SetMode(char *text,BOOL edit,BYTE *keys) {

  if( strlen(text)>=255 ) {
    strncpy(editText,text,255);
    editText[254]=' ';
    editText[255]=0;
  } else {
    strcpy(editText,text);
    strcat(editText," ");
  }
  startEditTime = 0.0f;
  editCursor = FALSE;
  editMode = edit;
  startPos = 0;
  editPos = 0;
  if(keys) ZeroMemory( keys, SDLK_LAST * sizeof(BYTE) );

}

// ------------------------------------------------

BOOL EditControl::GetMode() {
  return editMode;
}

// ------------------------------------------------

int EditControl::Process(BYTE *keys,float fTime) {

  int retValue = 0;

  if( startEditTime == 0.0f )
    startEditTime = fTime;

  editCursor = ( (fround((startEditTime - fTime) * 2.0f)%2) == 0 );

  char c = GetChar(keys);
  int lgth = (int)strlen(editText);
  if( c>0 && lgth<255 ) {
    InsertChar(c,editPos);
    editPos++;
    lgth++;
  }

  // Back space
  if( keys[SDLK_BACKSPACE] ) {
    if( editPos>0 ) {
      editPos--;
      DeleteChar(editPos);
      lgth--;
    }
    keys[SDLK_BACKSPACE] = 0;
  }

  // Delete
  if( keys[SDLK_DELETE] ) {
    if( editPos<lgth-1 ) {
      DeleteChar(editPos);
      lgth--;
    }
    keys[SDLK_DELETE] = 0;
  }

  // Arrow keys
  if( keys[SDLK_LEFT] ) {
    if( editPos>0 ) {
      editPos--;
    }
    keys[SDLK_LEFT] = 0;
  }

  if( keys[SDLK_RIGHT] ) {
    if( editPos<(lgth-1) ) {
      editPos++;
    }
    keys[SDLK_RIGHT] = 0;
  }

  // Scroll to visible
  if( editPos < startPos ) {
    startPos = editPos;
  }

  if( (editPos-startPos) >= displayLgth ) {
    startPos++;
  }

  if( keys[SDLK_ESCAPE] ) {
    // Cancelling
    editMode = FALSE;
    retValue = 2;
    ZeroMemory( keys, SDLK_LAST * sizeof(BYTE) );
    keys[SDLK_ESCAPE] = 0;
  }

  if( keys[SDLK_RETURN] ) {
    int x = 0;
    editText[lgth-1]=0; // Remove last ' '
    retValue = 1;
    editMode = FALSE;
    ZeroMemory( keys, SDLK_LAST * sizeof(BYTE) );
    keys[SDLK_RETURN] = 0;
  }

  return retValue;
}

// ------------------------------------------------

char *EditControl::GetText() {
  return editText;
}

// ------------------------------------------------

void EditControl::Render(Menu *m,int x,int y) {

  if( editMode ) {
    char vText[256];
    strcpy(vText,editText+startPos);
    vText[displayLgth]=0;
    m->RenderText(x,y,FALSE,vText);

    char vChar[2];
    vChar[0] = editText[editPos];
    vChar[1] = 0;
    m->RenderText(x+(editPos-startPos),y,editCursor,vChar);
  }

}

// ------------------------------------------------

void EditControl::InsertChar(char c,int pos) {

  int lgth = (int)strlen(editText);
  int i;
  for(i=lgth+1;i>pos;i--)
    editText[i] = editText[i-1];
  editText[i] = c;

}

// ------------------------------------------------

void EditControl::DeleteChar(int pos) {

  int lgth = (int)strlen(editText);
  for(int i=pos;i<lgth;i++)
    editText[i] = editText[i+1];

}
