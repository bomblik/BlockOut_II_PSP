//------------------------------------------------------------------------------
// Last updated: Nov. 16, 2000 
// Copyright (C) 1994-2000 Rich Geldreich
// richgel@voicenet.com
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//------------------------------------------------------------------------------
#include "jpegdecoder.h"

const bool use_mmx = true;

//------------------------------------------------------------------------------
// For debugging: purposely clear MMX state, to break any 'bad' code that depends
// on the regs being preserved across calls.
static void empty_mmx(void)
{
#if 0
  _asm
  {
    pxor mm0, mm0
    pxor mm1, mm1
    pxor mm2, mm2
    pxor mm3, mm3
    pxor mm4, mm4
    pxor mm5, mm5
    pxor mm6, mm6
    pxor mm7, mm7
    emms
  }
#endif
}

char JpegErrorMessage[1024];

//------------------------------------------------------------------------------
// Load a jpeg image return a 24Bit RGB bimtap
//------------------------------------------------------------------------------
int LoadJpegImage(JPEG_IMAGE *d) {

  const char *Psrc_filename = d->FileName;

  Pjpeg_decoder_file_stream Pinput_stream = new jpeg_decoder_file_stream();

  if( !Pinput_stream ) {
    sprintf(JpegErrorMessage,"Error: Unable to allocate memory for stream object!\n", Psrc_filename);
    return (FALSE);    
  }

  if (Pinput_stream->open(Psrc_filename))
  {
    delete Pinput_stream;
    sprintf(JpegErrorMessage,"Error: Unable to open file \"%s\" for reading!\n", Psrc_filename);
    return (FALSE);
  }

  Pjpeg_decoder Pd = new jpeg_decoder(Pinput_stream, use_mmx);

  if( !Pd ) {
    delete Pinput_stream;
    sprintf(JpegErrorMessage,"Error: Unable to allocate memory for decoder object!\n", Psrc_filename);
    return (FALSE);    
  }

  if (Pd->get_error_code() != 0)
  {
    sprintf(JpegErrorMessage,"Error: Decoder failed! Error status: %i\n", Pd->get_error_code());

    // Always be sure to delete the input stream object _after_
    // the decoder is deleted. Reason: the decoder object calls the input
    // stream's detach() method.
    delete Pd;
    delete Pinput_stream;

    return (FALSE);
  }

  d->width=Pd->get_width();
  d->height=Pd->get_height();

  // for debugging only!
  if (use_mmx)
    empty_mmx();

  if (Pd->begin())
  {
    sprintf(JpegErrorMessage,"Error: Decoder failed! Error status: %i\n", Pd->get_error_code());

    delete Pd;
    delete Pinput_stream;
    return (FALSE);
  }

  uchar *Pbuf = NULL;
  int    pitch = Pd->get_width() * 3;

  Pbuf = (uchar *)malloc(pitch);
  if (!Pbuf)
  {
    sprintf(JpegErrorMessage,"Error: Out of memory!\n");
    delete Pd;
    delete Pinput_stream;
    return (FALSE);
  }


  d->data = (char *)malloc( d->height*pitch );
  if( d->data==NULL ) {
    sprintf(JpegErrorMessage,"Error: Out of memory!\n");
    delete Pd;
    delete Pinput_stream;
    return (FALSE);
  }

  int lines_decoded = 0;

  for ( ; ; )
  {
    void *Pscan_line_ofs;
    uint scan_line_len;

    if (Pd->decode(&Pscan_line_ofs, &scan_line_len))
      break;

    // for debugging only!
    if (use_mmx)
      empty_mmx();

    if (Pd->get_num_components() == 3)
    {

      uchar *Psb = (uchar *)Pscan_line_ofs;
      uchar *Pdb = Pbuf;
      int src_bpp = Pd->get_bytes_per_pixel();

      for (int x = Pd->get_width(); x > 0; x--, Psb += src_bpp, Pdb += 3)
      {
        Pdb[0] = Psb[2];
        Pdb[1] = Psb[1];
        Pdb[2] = Psb[0];
      }

    }
    else
    {

      uchar *Psb = (uchar *)Pscan_line_ofs;
      uchar *Pdb = Pbuf;

	  for (int x = 0; x < d->width; x++,Pdb+=3,Psb++ ) {
		Pdb[0]=Psb[0];
		Pdb[1]=Psb[0];
		Pdb[2]=Psb[0];
	  }

    }

    memcpy( d->data + lines_decoded*pitch , Pbuf , pitch);
    lines_decoded++;
  
  }

  free(Pbuf);

  if (Pd->get_error_code())
  {
    sprintf(JpegErrorMessage,"Error: Decoder failed! Error status: %i\n", Pd->get_error_code());
    delete Pd;
    delete Pinput_stream;
    return (FALSE);
  }

  delete Pd;
  delete Pinput_stream;

  return (TRUE);
}

//------------------------------------------------------------------------------

