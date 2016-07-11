/*
   File:        EditControl.h
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

#ifndef EDITCONTROLH
#define EDITCONTROLH

class EditControl {

  public:
    EditControl();

    // Set display length
    void SetDisplayLength(int length);

    // Set edit control mode (keys can be NULL)
    void SetMode(char *text,BOOL edit,BYTE *keys);

    // Get the mode
    BOOL GetMode();

    // Process keys (Return 1 on RETURN, 2 on ESCAPE, 0 otherwise)
    int Process(BYTE *keys,float fTime);

    // Render
    void Render(Menu *m,int x,int y);

    // Return the text
    char *GetText();

 private:

    void InsertChar(char c,int pos);
    void DeleteChar(int pos);

    BOOL       editMode;
    char       editText[256];
    BOOL       editCursor;
    float      startEditTime;
    int        editPos;
    int        startPos;
    int        displayLgth;

};

#endif /* EDITCONTROLH */
