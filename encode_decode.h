#ifndef ENCODE_DECODE_H
#define ENCODE_DECODE_H
#endif
struct CompressImage {
int dec; 
int enc; 
int TCP_DISTORATIO;
int FILTER;  
int CR; 
int flg;
int bpp;
int imgsz;
int him;
int wim;
char *bufferptr;
};

//setting intial values in struct CompressImage
struct CompressImage s1 = {6,1,60,0,25,1,24,196608,256,256,0};
int dec, enc, TCP_DISTORATIO, FILTER, CR, flg, bpp;
  
int da_x0, da_y0, da_x1, da_y1;
char *fff;

long imgsz,him,wim, *bufferptr;
int COMPRESSION_RATIO;
void decom_test(int da_x0, int da_y0, int da_x1, int da_y1, char *ff_in);
char ff[]="test.j2k";
long imgsz,him,wim;
int TCP_DISTORATIO=60; 
int COMPRESSION_RATIO=1,CR = 25,ENCODE = 1;

typedef struct
    {
        unsigned char RGB[3];
    }RGB; 

//void lift_config(void lift_config(int dec, int enc, int TCP_DISTORATIO, int FILTER,  int CR, int flg, int bpp, long imgsz,long him,long wim, char bufferptr);
void lift_config(struct CompressImage *s);
RGB** createMatrix();
#pragma pack(push,1)
/* Windows 3.x bitmap file header */
typedef struct {
    char         filetype[2];   /* magic - always 'B' 'M' */
    unsigned int filesize;
    short        reserved1;
    short        reserved2;
    unsigned int dataoffset;    /* offset in bytes to actual bitmap data */
} file_header;

typedef struct {
    file_header  fileheader;
    unsigned int headersize;
    int          width;
    int          height;
    short        planes;
    short        bitsperpixel;  /* we only support the value 24 here */
    unsigned int compression;   /* we do not support compression */
    unsigned int bitmapsize;
    int          horizontalres;
    int          verticalres;
    unsigned int numcolors;
    unsigned int importantcolors;
} bitmap_header;
#pragma pack(pop)


