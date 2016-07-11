#ifndef _PNGH
#define _PNGH

typedef struct{

  char FileName[512];  /* PNG file Name               */
  int width;           /* Image width (pixels)        */
  int height;          /* Image height (pixels)       */
  char *data;		      /* Image data  24Bit BGR       */

} PNG_IMAGE;

#ifdef __cplusplus
extern "C" {
#endif

extern char PngErrorMessage[];
int LoadPngImage(PNG_IMAGE *d);
char *WritePngImage(char *file_name,unsigned long width,unsigned long height,unsigned char *data);

#ifdef __cplusplus
}
#endif

#endif
