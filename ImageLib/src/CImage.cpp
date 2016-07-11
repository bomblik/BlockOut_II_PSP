/***************************************************/
/* Unit GIF : contains procedure to write and load */
/*            conpressed gif image.                */
/*            Use an ultra fast compression method */
/***************************************************/
#include <stdio.h>

#include "CImage.h"
#include "gif/gif.h"
#include "jpg/jpegdecoder.h"
#include "png/hpng.h"

#ifdef WINDOWS
#define strcasecmp _stricmp
#endif

// ------ Default constructor -----

CImage::CImage() {

  strcpy(m_LastError,"No errors");
  m_Width=0;
  m_Height=0;
  m_Data=NULL;
 
}

// ------ Load image -----

int CImage::LoadImage(char *FileName) {

  Release();

  // Get extensions
  char *p = strrchr(FileName,'.');
  char ext[12];

  if( p==NULL ) {
    sprintf(m_LastError,"Does not have extensions.");
	return 0;  
  }

  strcpy(ext,p+1);

  if( strcasecmp(ext,"gif")!=0  &&
	  strcasecmp(ext,"jpg")!=0  &&
	  strcasecmp(ext,"jpeg")!=0 &&
	  strcasecmp(ext,"png")) {
    sprintf(m_LastError,"Not gif,jpg or png.");
	return 0;
  }

  // Load GIF image
  if( strcasecmp(ext,"gif")==0 ) {

    GIF_IMAGE img;
	int  Length;

	strcpy(img.FileName,FileName);
	if( !LoadGifImage(&img) ) {
	  strcpy(m_LastError,GifErrorMessage);
	  return 0;	
	}

    m_Width=img.width;
    m_Height=img.height;	
	Length=m_Width*m_Height;

	m_Data=(BYTE *)malloc( Length*3 );

	for(int i=0,j=0;i<Length;i++,j+=3) {
	  m_Data[j+0] = (BYTE)( img.Blue[   (BYTE)img.data[i] ] );
	  m_Data[j+1] = (BYTE)( img.Green[ (BYTE)img.data[i] ] );
	  m_Data[j+2] = (BYTE)( img.Red[  (BYTE)img.data[i] ] );
	}

	free(img.data);

	return 1;
      
  }

  // Load JPG image
  if( strcasecmp(ext,"jpg")==0 || strcasecmp(ext,"jpeg")==0 ) {

    JPEG_IMAGE img;

	strcpy(img.FileName,FileName);
	if( !LoadJpegImage(&img) ) {
	  strcpy(m_LastError,JpegErrorMessage);
	  return 0;	
	}

    m_Width=img.width;
    m_Height=img.height;	
	m_Data=(BYTE *)img.data;

	return 1;
      
  }

  // Load PNG image
  if( strcasecmp(ext,"png")==0 ) {

    PNG_IMAGE img;

	strcpy(img.FileName,FileName);
	if( !LoadPngImage(&img) ) {
	  strcpy(m_LastError,PngErrorMessage);
	  return 0;	
	}

    m_Width=img.width;
    m_Height=img.height;	
	m_Data=(BYTE *)img.data;

	return 1;
      
  }

  return 1;
}

int CImage::Width() {
  return m_Width;
}

int CImage::Height() {
  return m_Height;
}

BYTE *CImage::GetData() {
  return m_Data;
}

// ------ Get error message if a routine fails ------

char *CImage::GetErrorMessage() {
  return m_LastError;
}

// ------ Write a PNG file -----------------------------

char *CImage::WritePNG(char *FileName,int width,int height,BYTE *data) {
  return WritePngImage(FileName,width,height,data);
}

// ------ Release memory -----------------------------

void CImage::Release() {
  if( m_Data!=NULL ) {
    free(m_Data);
    m_Data=NULL;
  }
  m_Width=0;
  m_Height=0;
}

