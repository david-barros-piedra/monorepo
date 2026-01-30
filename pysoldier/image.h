#ifndef IMAGE_H_INCLUDE
#define IMAGE_H_INCLUDE

#include <X11/Xlib.h>
typedef struct {
    Pixmap pixmap;
    Pixmap mask;
    GC     maskgc;
    int    width, height;
} Image;

extern void ReadFileToImage(const char *filename, Image **img);
extern void PutImage(Image *img, int x, int y);
extern void SplitImage(Image *img, Image ***imgs, int nsplit);
extern void FreeImage(Image *img);
extern void FreeImages(Image **imgs, int nimg);
extern Image **ImageInit(const char *filename, int split);


#endif /* IMAGE_H_INCLUDE */
