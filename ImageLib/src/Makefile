#---------------------------------------------------------------------
# Makefile for ImageLib
#
# Note: Sorry to not have libtoolized this project but i don't have
#       enough time to do that.

#
#---------------------------------------------------------------------
# All source files needed for the library
#

SRC = 	gif/gif.cpp CImage.cpp png/zlib/adler32.c png/zlib/compress.c \
	png/zlib/crc32.c png/zlib/deflate.c png/zlib/gzio.c png/zlib/infback.c \
	png/zlib/inffast.c png/zlib/inflate.c png/zlib/inftrees.c png/zlib/trees.c \
	png/zlib/uncompr.c png/zlib/zutil.c png/png/png.c png/png/pngerror.c \
	png/png/pnggccrd.c png/png/pngget.c png/png/pngmem.c png/png/pngpread.c \
	png/png/pngread.c png/png/pngrio.c png/png/pngrtran.c png/png/pngrutil.c \
	png/png/pngset.c png/png/pngtrans.c png/png/pngvcrd.c png/png/pngwio.c \
	png/png/pngwrite.c png/png/pngwtran.c png/png/pngwutil.c png/hpng.c \
	jpg/H2v2.cpp jpg/idct.cpp jpg/jpegdecoder.cpp jpg/jpegdecodermain.cpp


OBJ=    gif/gif.o CImage.o png/zlib/adler32.o png/zlib/compress.o \
	png/zlib/crc32.o png/zlib/deflate.o png/zlib/gzio.o png/zlib/infback.o \
	png/zlib/inffast.o png/zlib/inflate.o png/zlib/inftrees.o png/zlib/trees.o \
	png/zlib/uncompr.o png/zlib/zutil.o png/png/png.o png/png/pngerror.o \
	png/png/pnggccrd.o png/png/pngget.o png/png/pngmem.o png/png/pngpread.o \
	png/png/pngread.o png/png/pngrio.o png/png/pngrtran.o png/png/pngrutil.o \
	png/png/pngset.o png/png/pngtrans.o png/png/pngvcrd.o png/png/pngwio.o \
	png/png/pngwrite.o png/png/pngwtran.o png/png/pngwutil.o png/hpng.o \
	jpg/H2v2.o jpg/idct.o jpg/jpegdecoder.o jpg/jpegdecodermain.o

#---------------------------------------------------------------------

CXX	= g++
CC	= gcc

CXXFLAGS    = -O2 -Dlinux -c -I.
CFLAGS      = -O2 -Dlinux -c -I.


#--------------------------------------------------------------------

all: libimagelib.a

libimagelib.a: $(OBJ)
	@echo Making library...
	@rm -f libimagelib.a
	ar -rv libimagelib.a $(OBJ)

clean:
	@echo Cleaning...
	@rm -f *.o
	@rm -f gif/*.o
	@rm -f jpg/*.o
	@rm -f png/*.o
	@rm -f png/png/*.o
	@rm -f png/zlib/*.o

