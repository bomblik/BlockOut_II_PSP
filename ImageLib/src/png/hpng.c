#include <malloc.h>
#include "math.h"
#include "png/png.h"
#include "hpng.h"

char PngErrorMessage[1024];

void my_png_error(png_structp png_ptr, png_const_charp message) {
  strcpy(PngErrorMessage,message);
}

// Load a PNG image -------------------------------------------

int LoadPngImage(PNG_IMAGE *d) {

	png_structp png;
	png_infop   info;
	png_infop   endinfo;
	png_bytep   data;
    png_bytep  *row_p;
    double	fileGamma;
	png_uint_32 width, height;
	int depth, color, bpp;
	png_uint_32 i,j;

	// ------------- Open the file

	FILE *fp = fopen(d->FileName, "rb");

	if (fp == NULL) {
	  sprintf(PngErrorMessage,"Unable to open the file %s.",d->FileName);
	  return 0;
	}

	// ---------------- Create PNG handle

	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if( png==NULL ) {
	  sprintf(PngErrorMessage,"Failed to create the PNG handle.");
	  return 0;
	}
	info = png_create_info_struct(png);
	endinfo = png_create_info_struct(png);

	// ---------------- Error handling

	png_set_error_fn(png,NULL,my_png_error,NULL);
  if (setjmp(png->jmpbuf)) {
 	  png_destroy_read_struct(&png, &info, &endinfo);
	  return 0;
	}

	// ----------------- Read the image

	png_init_io(png, fp);
	png_read_info(png, info);
	png_get_IHDR(png, info, &width, &height, &depth, &color, NULL, NULL, NULL);

	d->width  = width;
	d->height = height;

	if (color == PNG_COLOR_TYPE_GRAY || color == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);

	/*
	if (color&PNG_COLOR_MASK_ALPHA && trans != PNG_ALPHA) {
		png_set_strip_alpha(png);
		color &= ~PNG_COLOR_MASK_ALPHA;
	}
	*/

	if (color == PNG_COLOR_TYPE_PALETTE)
		png_set_expand(png);


	if (png_get_gAMA(png, info, &fileGamma))
		png_set_gamma(png, 2.2/1.0, fileGamma);
	else
		png_set_gamma(png, 2.2/1.0, 1.0/2.2);

	png_read_update_info(png, info);


	bpp = png_get_rowbytes(png, info)/width;
	data = (png_bytep) malloc(png_get_rowbytes(png, info)*height);
	row_p = (png_bytep *) malloc(sizeof(png_bytep)*height);

	for (i = 0; i < height; i++) {
//		row_p[height - 1 - i] = &data[png_get_rowbytes(png, info)*i];
		row_p[i] = &data[png_get_rowbytes(png, info)*i];
	}

	png_read_image(png, row_p);
	free(row_p);

    png_read_end(png, endinfo);
	png_destroy_read_struct(&png, &info, &endinfo);


	// Convert the image
	{
	  unsigned char *src = data;
	  unsigned char *dst = (unsigned char *)malloc(width*height*3);
	  d->data = dst;
	  for(i = 0 ; i < height ; i++) {
	    for(j = 0 ; j < width ; j++) {
		  dst[0] = src[2];
		  dst[1] = src[1];
		  dst[2] = src[0];
		  dst+=3;
		  src+=bpp;
		}
	  }
	}

	free(data);
	fclose(fp);

	return 1;

}

// ---------------------------------------------------------------------

char *WritePngImage(char *file_name,unsigned long width,unsigned long height,unsigned char *data) {

   FILE *fp;
   png_structp png_ptr;
   png_infop info_ptr;
   png_uint_32 k;
   png_bytep *row_pointers;

 	 // ------------- Open the file

   fp = fopen(file_name, "wb");
   if (fp == NULL) {
  	 sprintf(PngErrorMessage,"Unable to open the file %s for writting",file_name);
     return PngErrorMessage;
   }

   // ---------------- Create PNG handle

   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);

   if (png_ptr == NULL)
   {
    	sprintf(PngErrorMessage,"png_create_write_struct() failed");
      fclose(fp);
      return PngErrorMessage;
   }

   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
   {
    	sprintf(PngErrorMessage,"png_create_info_struct() failed");
      fclose(fp);
      png_destroy_write_struct(&png_ptr,  png_infopp_NULL);
      return PngErrorMessage;
   }

	 // ---------------- Error handling
   
   png_set_error_fn(png_ptr,NULL,my_png_error,NULL);
   
   if (setjmp(png_jmpbuf(png_ptr)))
   {
      fclose(fp);
      png_destroy_write_struct(&png_ptr, &info_ptr);
      return PngErrorMessage;
   }

   // ---------------- Write the file

   png_init_io(png_ptr, fp);
   png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB,
                PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
   png_write_info(png_ptr, info_ptr);
   png_set_bgr(png_ptr);

   row_pointers = (png_bytep *)malloc(height * sizeof(png_bytep));
   for (k = 0; k < height; k++)
     row_pointers[k] = data + (k*width*3);

   png_write_image(png_ptr, row_pointers);
   png_write_end(png_ptr, info_ptr);

   // Release allocated stuff
   free(row_pointers);
   png_destroy_write_struct(&png_ptr, &info_ptr);
   fclose(fp);

   /* that's it */
   return NULL;

}
