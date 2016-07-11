/*
   File:        Menu.h
  Description: Menu management
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

#include "MenuPage.h"
#include "PolyCube.h"
#include "MenuGrid.h"
#include "SetupManager.h"
#include "SoundManager.h"
#include "Http.h"

#define BLLETTER_NB 9
#define ANIMTIME    1.5f

class Menu {

  public:
    Menu();
    ~Menu();

    // Initialise device objects
    int Create(int width,int height);

    // Set/Grt view matrix
    void SetViewMatrix(GLfloat *mView);
    GLfloat *GetViewMatrix();

    // Render the menu
    void Render();

    // Release device objects
    void InvalidateDeviceObjects();

    // Process menu
    int Process(BYTE *keys,float fTime);

    // Set manager
    void SetSetupManager(SetupManager *manager);
    SetupManager *GetSetup();
    void SetSoundManager(SoundManager *manager);
    SoundManager *GetSound();
    void SetHttp(Http *h);
    Http *GetHttp();

    // Ask full repaint
    void FullRepaint();

    // Menu page
    void ToPage(MenuPage *page);
    void ToPage(MenuPage *page,int iParam,void *wParam);
    void RenderText(int x,int y,BOOL selected,char *text);
    void RenderTitle(char *title);

    PageMainMenu mainMenuPage;
    PageStartGame startGamePage;
    PageChooseSetup chooseSetupPage;
    PageChangeSetup changeSetupPage;
    PageHallOfFame hallOfFamePage;
    PageHallOfFameOnLine hallOfFamePageOnLine;
    PageOptions optionsPage;
    PageGSOptions gsOptionsPage;
    PageScoreDetails scoreDetailsPage;
    PageControls controlsPage;
    PageHttp httpPage;
    PageCredits creditsPage;

    // On-Line Logo
    Sprite2D onlineLogo;

  private:

    void CreatePage() {
      mainMenuPage.SetParent(this);
      startGamePage.SetParent(this);
      chooseSetupPage.SetParent(this);
      changeSetupPage.SetParent(this);
      hallOfFamePage.SetParent(this);
      optionsPage.SetParent(this);
      gsOptionsPage.SetParent(this);
      scoreDetailsPage.SetParent(this);
      controlsPage.SetParent(this);
      httpPage.SetParent(this);
      hallOfFamePageOnLine.SetParent(this);
      creditsPage.SetParent(this);
    };

    void InitGraphics();
    void ProcessAnim(float fTime);
    int  InitBlLetters();
    void ResetAnim(float fTime);
    void RenderChar(int x,int y,int w,int h,BOOL selected,char c);

    // Viewport and transformation matrix
    GLVIEWPORT menuView;
    GLfloat    matProj[16];
    GLfloat    matView[16];

    // Menu grid
    MenuGrid theGrid;

    // Menu background and font
    Sprite2D background;
    Sprite2D background2;
    Sprite2D font;
    Sprite2D font2;
    int wFont;
    int hFont;
    int scrWidth;
    int scrHeight;
    int fullRepaint;

    // Menu selection
    MenuPage *selPage;

    // Setup
    SetupManager *setupManager;
    SoundManager *soundManager;
    Http         *http;

    // Big Cube letters "BLOCKOUT"
    PolyCube  *blLetters[BLLETTER_NB];
    GLMatrix   blOrgMatrix;
    GLMatrix   blMatrix[BLLETTER_NB];
    VERTEX     startPos[BLLETTER_NB];
    VERTEX     endPos[BLLETTER_NB];
    BOOL       isAdded[BLLETTER_NB];
    BOOL       isStarted[BLLETTER_NB];
    BOOL       animEnded;
    BOOL       menuEscaped;

    // Time stamps
    float startMenuTime;

};
