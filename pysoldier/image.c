#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/xpm.h>

#include "image.h"
#include "xsoldier.h"
#include "extern.h"


static const char *XpmStatusToString(int status);

static const char *XpmStatusToString(int status){
    switch (status)
    {
      case XpmColorError:  return "XpmColorError";
      case XpmSuccess:     return "XpmSuccess";
      case XpmOpenFailed:  return "XpmOpenFailed";
      case XpmFileInvalid: return "XpmFileInvalid";
      case XpmNoMemory:    return "XpmNoMemory";
      case XpmColorFailed: return "XpmColorFailed";
    }
    return "Unknwon status";
}

void ReadFileToImage(const char *filename, Image **img) {
    XpmAttributes att;
    const char *FuncName = "ReadFileToImage";
    int status;
    /*
    *img = (Image *)malloc(sizeof(Image));
    */
    att.valuemask = XpmColormap;
    att.x_hotspot = 0U;
    att.y_hotspot = 0U;
    att.depth     = 8U;
    att.colormap  = cmap;

    /* I don't know why arg 3 of XpmReadFileToPixmap is not const */
    status = XpmReadFileToPixmap(dpy, WorkPixmap, (char *) filename, &((*img)->pixmap), &((*img)->mask), &att);
    if (status != XpmSuccess)
    {
	fprintf(stderr, "%s: [file error] can not read %s (%s)\n",
		FuncName, filename, XpmStatusToString(status));
	fflush(stderr);
	exit(1); 
    }
#ifdef DEBUG
    /* FIXME */
    if ((*img)->mask == None)
	fprintf(stderr, "%s: [format error] %s don't use None color\n", FuncName, filename);
#endif
    (*img)->width  = att.width;
    (*img)->height = att.height;

    if (((*img)->mask) != None)
	(*img)->maskgc = XCreateGC(dpy,(*img)->mask,0,0);
    else
	fprintf(stderr, "%s: [pixmap error] clip_mask is None!\n", FuncName);

    XpmFreeAttributes(&att);

    return;
}

void SplitImage(Image *img, Image ***imgs, int nsplit){
  int width = 0;
  int height = 0;
  int i;
#ifdef DEBUG
    const char *FuncName = "SplitImage";
#endif

    GC  gc8, gc1;

    width  = img->width;
    height = (img->height) / nsplit;
#ifdef DEBUG
    if ((img->height) % nsplit != 0)
    {
	fprintf(stderr, "%s: [warning] img->height (%d)/nsplit (%d) isn't just!\n", FuncName, img->height, nsplit);
	fflush(stderr);
    }
#endif

    gc8 = XCreateGC(dpy,img->pixmap, 0L,NULL);
    gc1 = XCreateGC(dpy,img->mask,   0L,NULL);

    XSetGraphicsExposures(dpy, gc8, False);
    XSetGraphicsExposures(dpy, gc1, False);

    (*imgs) = (Image **)malloc(sizeof(Image*)*nsplit);

    for (i=0; i<nsplit; i++)
	(*imgs)[i] = (Image *)malloc(sizeof(Image));

    for (i=0; i<nsplit; i++)
    {
	int x, y;

	x = 0;
	y = height * i;

	(*imgs)[i]->pixmap = XCreatePixmap(dpy,RootWindow(dpy,0),width,height,DefaultDepth(dpy,0));
	XCopyArea(dpy,img->pixmap,(*imgs)[i]->pixmap,gc8,x,y,width,height,0,0);

	(*imgs)[i]->mask = XCreatePixmap(dpy,RootWindow(dpy,0),width,height,1);
	XCopyArea(dpy,img->mask,  (*imgs)[i]->mask,gc1,x,y,width,height,0,0);

	(*imgs)[i]->maskgc = XCreateGC(dpy,WorkPixmap,0,0);
	XSetClipMask(dpy,(*imgs)[i]->maskgc,(*imgs)[i]->mask);

	(*imgs)[i]->width  = width;
	(*imgs)[i]->height = height;
    }

    XFreeGC(dpy,gc8);
    XFreeGC(dpy,gc1);
    XFlush(dpy);
    return;
}

void PutImage(Image *img, int x, int y) {
  XSetClipOrigin(dpy,img->maskgc,x,y);
  XCopyArea(dpy,img->pixmap,WorkPixmap,img->maskgc,0,0,img->width,img->height,x,y);
}

void FreeImage(Image *img) {
  XFreePixmap(dpy, img->pixmap);
  XFreePixmap(dpy, img->mask);
  XFreeGC(dpy, img->maskgc);
  free(img);
  return;
}

void FreeImages(Image **imgs, int nimg) {
  while (nimg) { FreeImage( imgs[ --nimg ] ); }
  free( imgs );
  return;
}

Image **ImageInit(const char *filename, int split) {
  Image*  Digit;
  Image** Digits;
  Digit = (Image *)malloc(sizeof(Image));
  ReadFileToImage( filename,&Digit);
  SplitImage(Digit,&Digits,split);
  FreeImage(Digit);
  return Digits;
}
