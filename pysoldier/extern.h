
#if !defined _EXTERN_H_
#define _EXTERN_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>

/* wait by signal */
int signal_delivered;
int waittime;


/* for XWindow */
Display *dpy;
Colormap cmap;
Window   root;
Window   win;
Pixmap   WorkPixmap;
XEvent   event;

Font     font;
GC       FontGC;
GC       BackGC;
GC       FillGC; /* GC for debugging rectangles */
XColor   black;
XColor   white;



int      keymask;
int      joymask;

char *upKey;
char *downKey;
char *leftKey;
char *rightKey;
char *shotKey;
char *spdupKey;
char *spdwnKey;
char *pauseKey;
char *quitKey;

/* character management */
CharManage *manage;

/* player data (score.. stage...) */
PlayerData *player;

/* field */
#define  FieldW (500)
#define  FieldH (650)

/* star */
int StarPtn1;
int StarPtn2;

/* image */
Image **PlayerImage;
Image **PShot1Image;
Image **PShot2Image;
Image **PShot3Image;

Image **EShotImage;
Image **ELaserImage;
Image **EMissileImage;
Image **EBoundImage;
Image **ERingImage;

Image **BombImage;
Image **LargeBombImage;

Image **Enemy1Image;
Image **Enemy2Image;
Image **Enemy3Image;
Image **Enemy4Image;
Image **Enemy5Image;
Image **Enemy6Image;
Image **Enemy7Image;

Image **Boss1Image;
Image **Boss2Image;
Image **Boss3Image;
Image **Boss4Image;
Image **Boss5Image;
Image **Boss6Image;
Image **Boss7Image;

Image **ItemImage;

#endif
