/***************************************************/
/* CImage class: Support GIF,PNG and JPEG format   */
/***************************************************/
#ifndef _CIMAGEH_
#define _CIMAGEH_

#include <windows.h>

class CImage {

public:

	// Default constructor
	CImage();

	// Load an image
	// Supports GIF , JPG or PNG format
	// Return 1 when image has been succesfully load , 0 otherwise.
  int LoadImage(char *FileName);

	// Get error message if LoadImage fails.
	char *GetErrorMessage();

	// Returns image width
	int Width();

	// Returns image height
	int Height();

	// Returns data (24Bits BGR)
	BYTE *GetData();

	// Release memory
	void Release();

  // Write a PNG file (Return an error message when fail, NULL otherwise)
  // Data are 24Bits BGR
  static char *WritePNG(char *FileName,int width,int height,BYTE *data);

private:

	char  m_LastError[1024];      /* Error message               */
	BYTE *m_Data;                 /* Handle to data (24Bits BGR) */
  int   m_Width;                /* Image width (pixels)        */
  int   m_Height;               /* Image height (pixels)       */

};

#endif