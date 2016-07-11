/***************************************************/
/* Unit GIF : contains procedure to write and load */
/*            conpressed gif image.                */
/*            Use an ultra fast compression method */
/***************************************************/
#ifndef GIFH
#define GIFH

typedef struct{
  char FileName[512];         /* Gif file Name               */
  int width;                  /* Image width (pixels)        */
  int height;                 /* Image height (pixels)       */
  short Red[256];             /* Red colormap                */
  short Green[256];           /* Green colormap              */
  short Blue[256];            /* Blue colormap               */
  int pixel;                  /* bytes per pixel             */
  int background;             /* background color            */
  int transparent;            /* Transparent color (FC 249)  */
  int format;                 /* Image format                */
  int unique_color;           /* number of unique colors     */
  int used_color;	      /* number of used colors       */
  char *data;		      /* Image data                  */
  int  Used[256];	      /* Color tag                   */
} GIF_IMAGE;

#define INTERLACED_FORMAT 1
#define CONTINOUS_FORMAT  2

extern char GifErrorMessage[];

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************/
/* LoadGifImage : Load a gif file and return uncompressed  */
/*                data (malloc)                            */
/*                GIF_IMAGE *descr - GIF_FILE descriptor   */
/*                return 0 when fail                       */
/***********************************************************/
extern int LoadGifImage(GIF_IMAGE *descr);

/************************************************************/
/* ComupteUsedColor() : Compute .Used et .unique field      */
/*                GIF_IMAGE *descr - GIF_FILE descriptor   */
/************************************************************/
extern void ComputeUsedColor(GIF_IMAGE *descr);

/***********************************************************/
/* SaveGifImage : Save a gif file.                         */
/*                GIF_IMAGE descr - GIF_FILE descriptor    */
/*                return 0 when fail                       */
/***********************************************************/
extern int SaveGifImage(GIF_IMAGE);

#ifdef __cplusplus
}
#endif

#endif
