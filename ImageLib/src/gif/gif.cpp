/***************************************************/
/* Unit GIF : contains procedure to write and load */
/*            conpressed gif image.                */
/*            Use an ultra fast compression method */
/***************************************************/
#include <stdio.h>
#include <malloc.h>
//#include <memory.h>
#include <string.h>
#include "gif.h"

#define MAX_GIF_SIZE  1048576    /* Max of comprimed data */
#define GIF_SEPARATOR ','
#define GIF_EXTENDED  '!'

/***************************************************************/

FILE *GIF_FILE;
static unsigned int GIF_WIDTH;
static unsigned int GIF_HEIGHT;
static unsigned int GIF_BYTES_PER_PIXEL;
static unsigned int GIF_DATA_RESOLUTION;
static unsigned int GIF_BACK;
static          int GIF_TRANSPARENT;
static unsigned int GIF_FORMAT;
unsigned char GIF_RCOLORMAP[256];
unsigned char GIF_GCOLORMAP[256];
unsigned char GIF_BCOLORMAP[256];
static unsigned char *GIF_BUFFER=NULL;

char GifErrorMessage[1024];

#ifdef WITH_COMP
short GIF_REL[4096][256];
#endif

/***************** Variables used by GetCode *******************/

static unsigned int	  POS_CODE;
static unsigned long  MASK_CODE;
static unsigned int   SIZE_CODE;
static unsigned long  WORK_CODE;
static unsigned int	  WORK_CODE_SIZE;

/******************Vars of the analyseur ***********************/

static unsigned int EOF_CODE;
static unsigned int CLR_CODE;
static unsigned short NULL_CODE=0xFFFF;

/*****************Vars for table********************************/
static unsigned int TABLE_SIZE;
static int Prefix[4096];
static int Values[4096];
static int Length[4096];

static int buf_size=0;
static int Len=0;
static char *TheImage;

/************************************************************/
/* InitTable() : Initialise table string                    */
/************************************************************/
void InitTable() {
int i;
  for(i=0;i<4096;i++) { 
    Prefix[i]=NULL_CODE;
	Values[i]=i;
	Length[i]=1; 
  }
}


/***************************************************************/
/* ReadGifHeader : Read gif header                             */
/***************************************************************/
int ReadGifHeader() 
{
  char tmp[128];
  unsigned int lng;
  unsigned long lue;
  unsigned short c16;
  unsigned char  c8;
  int i;
  int eof_extension;
  int bc;
  int cmd_code;

  lng=6;

  /************************ Signature *************************/

  lue = (unsigned long)fread(tmp, 1, lng, GIF_FILE);
  if(lue!=lng) return 0;
  tmp[3]=0;
  if(strcmp(tmp,"GIF")!=0)
  {
    strcat(GifErrorMessage,"Bad gif signature.\n");
    return 0;
  }

  /************************ Screen descriptor ****************/

  lng=sizeof(short);
  lue = (unsigned long)fread(&c16, 1, lng, GIF_FILE);
  if(lue!=lng) return 0;

  lng=sizeof(short);
  lue = (unsigned long)fread(&c16, 1, lng, GIF_FILE);
  if(lue!=lng) return 0;

  lng=sizeof(char);
  lue = (unsigned long)fread(&c8, 1, lng, GIF_FILE);
  if(lue!=lng) return 0;
  GIF_BYTES_PER_PIXEL=(c8 & 0x07)+1;
  GIF_DATA_RESOLUTION=((c8 & 0x70)>>3)+1;

  lng=sizeof(char);
  lue = (unsigned long)fread(&c8, 1, lng, GIF_FILE);
  if(lue!=lng) return 0;
  GIF_BACK=(int)c8;

  lng=sizeof(char);
  lue = (unsigned long)fread(&c8, 1, lng, GIF_FILE);
  if(lue!=lng) return 0;

  /*********************** Global color map ****************/

  for(i=0;i<(1 << GIF_BYTES_PER_PIXEL);i++)
  {

    lng=sizeof(char);
    
	lue = (unsigned long)fread(&c8, 1, lng, GIF_FILE);
    if(lue!=lng) return 0;
    GIF_RCOLORMAP[i]=c8;
   
	lue = (unsigned long)fread(&c8, 1, lng, GIF_FILE);
    if(lue!=lng) return 0;
    GIF_GCOLORMAP[i]=c8;
  
	lue = (unsigned long)fread(&c8, 1, lng, GIF_FILE);
    if(lue!=lng) return 0;
    GIF_BCOLORMAP[i]=c8;

  }

  /**************************** Extended function ***************/

  eof_extension=0;

  while(!eof_extension)
  {
	  lue = (unsigned long)fread(&c8, 1, lng, GIF_FILE);
      if(lue!=lng) return 0;

    switch(c8) {
      case GIF_SEPARATOR:
	   eof_extension=1;
           break;

      case GIF_EXTENDED:

		    lue = (unsigned long)fread(&c8, 1, lng, GIF_FILE);
           if(lue!=lng) return 0;
	   cmd_code=(int)c8;

	   do {
		     lue = (unsigned long)fread(&c8, 1, lng, GIF_FILE);
             if(lue!=lng) return 0;
	     bc=(int)c8;

	     for(i=0;i<bc;i++) {
			   lue = (unsigned long)fread(&c8, 1, lng, GIF_FILE);
               if(lue!=lng) return 0;
	     }

	     if(cmd_code==249) {
	       GIF_TRANSPARENT=(int)c8;
	       cmd_code=-1;
	     }

	   } while(bc!=0);

           break;

      default:
           strcat(GifErrorMessage,"Unexcepted extension code in gif file.\n");
           return 0;
	   break;
    }
  }


  /**************************** First image descriptor **********/

 
  lng=sizeof(short);
  lue = (unsigned long)fread(&c16, 1, lng, GIF_FILE);
  if(lue!=lng) return 0;

  lue = (unsigned long)fread(&c16, 1, lng, GIF_FILE);
  if(lue!=lng) return 0;
 
  
  lue = (unsigned long)fread(&c16, 1, lng, GIF_FILE);
  if(lue!=lng) return 0;
  GIF_WIDTH=c16;

  lue = (unsigned long)fread(&c16, 1, lng, GIF_FILE);
  if(lue!=lng) return 0;
  GIF_HEIGHT=c16;

  
  lng=sizeof(char);
  lue = (unsigned long)fread(&c8, 1, lng, GIF_FILE);
  if(lue!=lng) return 0;
  
  if(c8 & 0x40)
     GIF_FORMAT=INTERLACED_FORMAT;
  else 
     GIF_FORMAT=CONTINOUS_FORMAT;

  SIZE_CODE=GIF_BYTES_PER_PIXEL+1;
  WORK_CODE_SIZE=0;
  WORK_CODE=0;
  MASK_CODE=((1 << SIZE_CODE)-1);
  CLR_CODE=1 << GIF_BYTES_PER_PIXEL;
  EOF_CODE=CLR_CODE+1;
  TABLE_SIZE = EOF_CODE + 1;
  POS_CODE=0;
  
  InitTable();

  /******************* Jump code size **********/
  /************* Supposed equal to 8 ***********/

  lng=sizeof(char);
  lue = (unsigned long)fread(&c8, 1, lng, GIF_FILE);
  if(lue!=lng) return 0;
 
  return 1;
}


/*************************************************************/
/* InitBlocks() : initialise a raster data block             */
/*************************************************************/
int InitBlocks() {
unsigned long lue,lng;
unsigned char c8=1;
int cnt=0;

while(c8!=0) {
  lng=sizeof(char);
  lue = (unsigned long)fread(&c8, 1, lng, GIF_FILE);
  if(lue!=lng) return 0;

  if(buf_size+c8>MAX_GIF_SIZE) {
    strcat(GifErrorMessage,"GIF file too long.\n");
    return 0;
  }

  if(c8>0) {
    cnt++;
    lng=c8;
	lue = (unsigned long)fread(&GIF_BUFFER[buf_size], 1, lng, GIF_FILE);
    if(lue!=lng) return 0;
    buf_size+=lng;
  }
}


return 1;
}

/************************************************************/
/* GetCode():return the current code                        */
/************************************************************/
int GetCode()
{
  int   ret;

  while(WORK_CODE_SIZE<SIZE_CODE)
  {
    WORK_CODE |= ((long)(GIF_BUFFER[POS_CODE]) << WORK_CODE_SIZE);
    POS_CODE++;
    WORK_CODE_SIZE+=8;
  }

  ret=WORK_CODE & MASK_CODE;
  WORK_CODE>>=SIZE_CODE;
  WORK_CODE_SIZE-=SIZE_CODE;
  return ret;
}

/************************************************************/
/* Output(): print the string for code and return the first */
/*           code                                           */
/************************************************************/
int Output(int code) {

  int i;
  int l;
  int r;

  l=Length[code];

  for(i=l-1;i>=0;i--) {
    TheImage[Len+i]=Values[code];
    code=Prefix[code];
  }

  r=TheImage[Len];
  Len+=l;
  return r;
}



/************************************************************/
/* AddString() add a string into the string table.          */
/************************************************************/

void Add_String(int p,int v) {
  Prefix[TABLE_SIZE]=p;
  Values[TABLE_SIZE]=v;
  Length[TABLE_SIZE]=Length[p]+1;

  if( (TABLE_SIZE == MASK_CODE ) && (SIZE_CODE<12))
  {
    SIZE_CODE++;
    MASK_CODE=((1 << SIZE_CODE)-1);
  }

  TABLE_SIZE++;
}



/************************************************************/
/* ReadRasterData : ReadFile the compressed data            */
/************************************************************/
int ReadRasterData()
{
  unsigned int code;
  int prev_code=NULL_CODE;
  int first;

  /*************** Read all gif blocks in memory ******/
  if(!InitBlocks()) return 0;

  /*************** Initialise analyseur ***************/

  code=GetCode();
  if(code==CLR_CODE) code=GetCode();
  Output(code);
  prev_code=code;

  /*************** Start decompression ***************/

  while(code!=EOF_CODE)
  {
  code=GetCode();

  /******************** Treat sepcial gif code *******/

  if( code==CLR_CODE || code==EOF_CODE) {
    SIZE_CODE=GIF_BYTES_PER_PIXEL+1;
    MASK_CODE=((1 << SIZE_CODE)-1);
    TABLE_SIZE = EOF_CODE + 1;

    if(code==CLR_CODE) {
      code=GetCode();
      Output(code); 
      prev_code=code;
    }

  } else

  /********************** LZW code *******************/

    if( code < TABLE_SIZE ) {
      first=Output(code);
      Add_String(prev_code,first);
      prev_code=code;
    } else {
      first=Output(prev_code);
      TheImage[Len++]=first;
      Add_String(prev_code,first);
      prev_code=code;
    }

 }

 return 1;
}

/***********************************************************/
/* LoadGifImage : Load a gif file and return uncompressed  */
/*                data (malloc)                            */
/*                GIF_IMAGE *descr - GIF_FILE descriptor   */
/*                return 0 when fail                       */
/***********************************************************/
int LoadGifImage(GIF_IMAGE *descr)
{
  int i,j;

  unsigned long TblCol[256];
  int trouve;
  unsigned long current;
  char *tmp_img;
  int addd,adds;

  /* Initialise default values */
  if(!GIF_BUFFER) GIF_BUFFER = (unsigned char *)malloc(MAX_GIF_SIZE);
  Len=0;
  buf_size=0;
  GifErrorMessage[0]='\0';

  for(i=0;i<256;i++)
  {
    descr->Red[i]=-1;
    descr->Green[i]=-1;
    descr->Blue[i]=-1;
    GIF_TRANSPARENT=-1;
  }
  GIF_FORMAT=CONTINOUS_FORMAT;
    
  /* Open file for Reading */

  GIF_FILE=fopen(descr->FileName,"rb");

  if (GIF_FILE==NULL)
  {
    sprintf(GifErrorMessage,"Unable to open file.\n%s",descr->FileName);
    descr->data=NULL;
    descr->width=0;
    descr->height=0;
    return 0;
  }

  /* Read the header */

 if(!ReadGifHeader())  {
    strcat(GifErrorMessage,"Error occurs during reading gif header.");
    descr->data=NULL;
    descr->width=0;
    descr->height=0;
    return 0;
  }

  /* Update descritpor */

  descr->data=(char *)malloc(GIF_WIDTH*GIF_HEIGHT);
  if( descr->data == NULL ) {
    strcat(GifErrorMessage,"Not enough memory to read GIF.");
    descr->width=0;
    descr->height=0;
    return 0;
  }

  TheImage=descr->data;
  descr->width=GIF_WIDTH;
  descr->height=GIF_HEIGHT;
  descr->background=GIF_BACK;
  descr->pixel=GIF_BYTES_PER_PIXEL;
  descr->transparent=GIF_TRANSPARENT;

  for(i=0;i<(1 << GIF_BYTES_PER_PIXEL);i++) {
    descr->Red[i]  =GIF_RCOLORMAP[i];
    descr->Green[i]=GIF_GCOLORMAP[i];
    descr->Blue[i] =GIF_BCOLORMAP[i];
  }
  
  descr->format=GIF_FORMAT;

  /* Unpacked comprimed blocks */

  if(!ReadRasterData()) {
    strcat(GifErrorMessage,"Error occurs during reading raster data.\n");
    free(descr->data);
    descr->data=NULL;
    descr->width=0;
    descr->height=0;
    return 0;
  }

  /* Regenerate picture when interlaced format */

  if(descr->format==INTERLACED_FORMAT) {

    tmp_img=(char *)malloc(GIF_WIDTH*GIF_HEIGHT);
	if( tmp_img == NULL ) {
      strcat(GifErrorMessage,"Not enough memory to read interlaced GIF.");
      free(descr->data); 
      descr->data=NULL;
      descr->width=0;
      descr->height=0;
      return 0;
	}
    memcpy(tmp_img,TheImage,GIF_WIDTH*GIF_HEIGHT);

    /* Fisrt passe */

    for(j=0,i=0;i<(int)GIF_HEIGHT;i+=8,j++) {
      adds=(j*GIF_WIDTH);
      addd=(i*GIF_WIDTH);
      memcpy(&TheImage[addd],&tmp_img[adds],GIF_WIDTH);
    }
    
    /* Second passe */

    for(i=4;i<(int)GIF_HEIGHT;i+=8,j++) {
      adds=(j*GIF_WIDTH);
      addd=(i*GIF_WIDTH);
      memcpy(&TheImage[addd],&tmp_img[adds],GIF_WIDTH);
    }
    
    /* Third passe */

    for(i=2;i<(int)GIF_HEIGHT;i+=4,j++) {
      adds=(j*GIF_WIDTH);
      addd=(i*GIF_WIDTH);
      memcpy(&TheImage[addd],&tmp_img[adds],GIF_WIDTH);
    }
    
    /* Fourth passe */

    for(i=1;i<(int)GIF_HEIGHT;i+=2,j++) {
      adds=(j*GIF_WIDTH);
      addd=(i*GIF_WIDTH);
      memcpy(&TheImage[addd],&tmp_img[adds],GIF_WIDTH);
    }

    free(tmp_img);


  }

  /* Compute Used color */
  j=GIF_WIDTH*GIF_HEIGHT;
  for(i=0;i<256;i++) descr->Used[i]=0;
  for(i=0;i<j;i++) descr->Used[(unsigned char)TheImage[i]]++;

  /* Compute Unique color */

  descr->used_color=0;
  descr->unique_color=0;

  for(i=0;i<256;i++)
    if(descr->Used[i]>0) {
      descr->used_color++;
      trouve=0;
      j=0;
      current= GIF_RCOLORMAP[i] +
	      (((long)GIF_GCOLORMAP[i]) << 8) + 
	      (((long)GIF_BCOLORMAP[i]) << 16);

      while( j<descr->unique_color && !trouve )
      {
	    trouve=(current==TblCol[j]);
	    if(!trouve) j++;
      }

      if(!trouve) {
	TblCol[j]=current;
	descr->unique_color++;
      }
    }


  fclose(GIF_FILE);
  return -1;
}

/************************************************************/
/* ComupteUsedColor() : Compute .Used et .unique field      */
/************************************************************/
void ComputeUsedColor(GIF_IMAGE *descr) {
  int i,j;
  unsigned long TblCol[256];
  int trouve;
  unsigned long current;
  
  
  /* Compute Used color */
  j=descr->width*descr->height;
  for(i=0;i<256;i++) descr->Used[i]=0;
  for(i=0;i<j;i++) descr->Used[(unsigned char)(descr->data[i])]++;

  /* Compute Unique color */

  descr->used_color=0;
  descr->unique_color=0;

  for(i=0;i<256;i++)
    if(descr->Used[i]>0) {
      descr->used_color++;
      trouve=0;
      j=0;
      current= descr->Red[i] +
	      (((long)descr->Green[i]) << 8) + 
	      (((long)descr->Blue[i]) << 16);

      while( j<descr->unique_color && !trouve )
      {
	trouve=(current==TblCol[j]);
	if(!trouve) j++;
      }

      if(!trouve) {
	TblCol[j]=current;
	descr->unique_color++;
      }
    }

}

/*************************************************************/
/* Compression Part                                          */
/*************************************************************/
#ifdef WITH_COMP

void Add_String2(int p,int v) {

  GIF_REL[p][v]=TABLE_SIZE;

  if( (TABLE_SIZE == (MASK_CODE+1) ) && (SIZE_CODE<12))
  {
    SIZE_CODE++;
    MASK_CODE=((1 << SIZE_CODE)-1);
  }

  TABLE_SIZE++;
}

void InitGifRelation() {
  memset( GIF_REL , NULL_CODE , 4096*512 );
}

void PutCode(int code)
{
	WORK_CODE |= (long)code << WORK_CODE_SIZE;
	WORK_CODE_SIZE+=SIZE_CODE;

	while( WORK_CODE_SIZE >= 8 ) {

		GIF_BUFFER[POS_CODE]=(unsigned char)(WORK_CODE & 0x000000FF);
		WORK_CODE >>= 8;
		WORK_CODE_SIZE -=8;
		POS_CODE++;
	}
}

int IsInTheStringTable( unsigned int p,unsigned int c,unsigned int *ind)
{
  int trouve;
  trouve=0;

  if(p==NULL_CODE) {
    *ind=c;
    return 1;
  } else {
    *ind=(int)GIF_REL[p][c];
    return (*ind!=NULL_CODE);
  }

}



/************************************************************/
void ComprimeData(int size_src   ,char *source,
		 int *size_dest )
{ 
 unsigned int prev_code;
 unsigned int code;
 unsigned int indice;

  Len=0;
  GIF_BYTES_PER_PIXEL=8;
  SIZE_CODE=9;
  WORK_CODE_SIZE=0;
  WORK_CODE=0;
  MASK_CODE=((1 << SIZE_CODE)-1);
  CLR_CODE=1 << GIF_BYTES_PER_PIXEL;
  EOF_CODE=CLR_CODE+1;
  TABLE_SIZE = EOF_CODE + 1;
  POS_CODE=0;
  

  InitGifRelation();
  prev_code=NULL_CODE;
  PutCode(CLR_CODE);

  while( Len<size_src ) {

    code=(unsigned char)source[Len++];

    if(IsInTheStringTable(prev_code,code,&indice)) {
      prev_code=indice;
    } else {
      PutCode(prev_code);

      if( TABLE_SIZE == 4096 ) {
	PutCode(CLR_CODE);
	SIZE_CODE=9;
        MASK_CODE=((1 << SIZE_CODE)-1);
        TABLE_SIZE = EOF_CODE + 1;
        InitGifRelation();
      } else
        Add_String2(prev_code,code);

      prev_code=code;

    }

  }

  PutCode(prev_code);
  PutCode(EOF_CODE);

  /* Flush the buffer */

  while( WORK_CODE_SIZE > 0 ) {
	GIF_BUFFER[POS_CODE]=(unsigned char)(WORK_CODE & 0x000000FF);
	WORK_CODE >>= 8;
	WORK_CODE_SIZE -=8;
	POS_CODE++;
  }


  *size_dest=POS_CODE;
}



/***********************************************************/
/* WriteGifHeader : Write a gif header.                    */
/*                GIF_IMAGE descr - GIF_FILE descriptor    */
/*                return 0 when fail                       */
/***********************************************************/
int WriteGifHeader(GIF_IMAGE descr) {

int i;
unsigned long wrt,lng;
unsigned short c16;
unsigned char  c8;
char tmp[100];

  /************************ Signature *************************/

  sprintf(tmp,"GIF89a");
  lng=6;
  WriteFile(GIF_FILE,tmp,lng,&wrt,NULL);
  if(wrt!=lng) return 0;

  /************************ Screen descriptor ****************/

  lng=sizeof(short);
  c16=(unsigned short)descr.width;
  WriteFile(GIF_FILE,&c16,lng,&wrt,NULL);
  if(wrt!=lng) return 0;

  lng=sizeof(short);
  c16=(unsigned short)descr.height;
  WriteFile(GIF_FILE,&c16,lng,&wrt,NULL);
  if(wrt!=lng) return 0;

  lng=sizeof(char);
  c8=0xF7;
  WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
  if(wrt!=lng) return 0;

  lng=sizeof(char);
  c8=descr.background;
  WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
  if(wrt!=lng) return 0;

  lng=sizeof(char);
  c8=0;
  WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
  if(wrt!=lng) return 0;

  /********************************** global ColorMap ****************/

  lng=sizeof(char);
  for(i=0;i<256;i++)
  {
    c8=(unsigned char)descr.Red[i];
    WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
    if(wrt!=lng) return 0;
   
    c8=(unsigned char)descr.Green[i];
    WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
    if(wrt!=lng) return 0;
  
    c8=(unsigned char)descr.Blue[i];
    WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
    if(wrt!=lng) return 0;

  }

  /********************************** Extension blocks **************/

  if(descr.transparent>=0) {

    c8=GIF_EXTENDED;
    WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
    if(wrt!=lng) return 0;
    c8=249;
    WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
    if(wrt!=lng) return 0;
    c8=4;
    WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
    if(wrt!=lng) return 0;
    c8=1;
    WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
    if(wrt!=lng) return 0;
    c8=0;
    WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
    if(wrt!=lng) return 0;
    c8=0;
    WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
    if(wrt!=lng) return 0;
    c8=(unsigned char)descr.transparent;
    WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
    if(wrt!=lng) return 0;
    c8=0;
    WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
    if(wrt!=lng) return 0;

  }

  /************************************* Image descriptor ************/

  c8=GIF_SEPARATOR;
  WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
  if(wrt!=lng) return 0;
  
  lng=sizeof(short);
  
  c16=0;
  WriteFile(GIF_FILE,&c16,lng,&wrt,NULL);
  if(wrt!=lng) return 0;
  
  WriteFile(GIF_FILE,&c16,lng,&wrt,NULL);
  if(wrt!=lng) return 0;
  
  c16=(unsigned short)descr.width;
  WriteFile(GIF_FILE,&c16,lng,&wrt,NULL);
  if(wrt!=lng) return 0;

  c16=(unsigned short)descr.height;
  WriteFile(GIF_FILE,&c16,lng,&wrt,NULL);
  if(wrt!=lng) return 0;

  c8=(descr.pixel-1);

  lng=sizeof(char);
  WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
  if(wrt!=lng) return 0;

  return 1;
}

/***********************************************************/
/* WriteBloks : Write blocks into file.                    */
/*              int pack_size : lenght of buffer to write  */
/*                return 0 when fail                       */
/***********************************************************/
int WriteBlocks(int pack_size) {

  int p_buf;
  unsigned char c8;
  unsigned long wrt,lng;
  int fin=0;

  /************* Write size code *********/

  lng=sizeof(char);
  c8=8;
  WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
  if(wrt!=lng) return 0;
  p_buf=0;

  while(!fin) {

    if( pack_size-p_buf >= 255 ) {

      lng=sizeof(char);
      c8=255;
      WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
      if(wrt!=lng) return 0;

      lng=255;
      WriteFile(GIF_FILE,&(GIF_BUFFER[p_buf]),lng,&wrt,NULL);
      if(wrt!=lng) return 0;
      p_buf+=255;

    } else {

      c8=(unsigned char)(pack_size-p_buf);
      
      if(c8>0) {
        lng=sizeof(char);
        WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
        if(wrt!=lng) return 0;

        lng=c8;
        WriteFile(GIF_FILE,&(GIF_BUFFER[p_buf]),lng,&wrt,NULL);
        if(wrt!=lng) return 0;
      }

      lng=sizeof(char);
      c8=0;
      WriteFile(GIF_FILE,&c8,lng,&wrt,NULL);
      if(wrt!=lng) return 0;
      fin=1;
    }
  }

  return 1;
}


/***********************************************************/
/* SaveGifImage : Save a gif file.                         */
/*                GIF_IMAGE descr - GIF_FILE descriptor    */
/*                return 0 when fail                       */
/***********************************************************/
int SaveGifImage(GIF_IMAGE descr)
{
  int packed_size;

  /* Init default */

  if(!GIF_BUFFER) GIF_BUFFER = (unsigned char *)malloc(MAX_GIF_SIZE);
  GifErrorMessage[0]='\0';
  
  if (descr.width==0 || descr.height==0)
  {
    strcat(GifErrorMessage,"No picture to save.\n");
    return 0;
  }

  /* Open file for writing */
  
  GIF_FILE=CreateFile(descr.FileName,
		              GENERIC_WRITE, 
					  0,
					  NULL,
					  CREATE_ALWAYS,
					  FILE_ATTRIBUTE_NORMAL,
					  NULL);


  if (GIF_FILE==INVALID_HANDLE_VALUE)
  {
    strcat(GifErrorMessage,"Unable to open file for writing.");
    return 0;
  }

  /* Write Gif header */

  if(WriteGifHeader(descr)==0) {
    strcat(GifErrorMessage,"Error when writing gif header.");
    return 0;
  }

  /* Comprime data */

  ComprimeData ( descr.width*descr.height , descr.data , &packed_size );

  /* Write into Blocks */

  if(WriteBlocks(packed_size)==0) {
    strcat(GifErrorMessage,"Error when writing gif blocks.");
    return 0;
  }

  CloseHandle(GIF_FILE);
  return -1;
}

#endif /* WITH_COMP */
