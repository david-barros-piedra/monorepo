#include <config.h>

#include <stdio.h>
/* exit  */
#include <stdlib.h>
/* isprint */
#include <ctype.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/xpm.h>

/* Image */
#include "image.h"
#include "graphic.h"
#include "xsoldier.h"

#include "extern.h"


#if 0
static GC FontGC;
static GC BackGC;
static GC FillGC;
static XColor black;
static XColor white;

static Display *dpy;
static Colormap cmap;
static Window root;
static Window win;
static Pixmap WorkPixmap;
#endif /* 0 */

static Image **Font1Image;
static Image **Font2Image;
static Image **Font3Image;
static Image **Font4Image;
static Image **Font5Image;
static Image **Font6Image;


int graphic_init( void ) {
  XColor blackTrue;
  XColor whiteTrue;
  XSizeHints sh;


  dpy = XOpenDisplay('\0');
  if (dpy == NULL) {
    fprintf(stderr,"graphic_init: can't open display.\n");
    exit(1);
  }

  cmap = XCreateColormap(dpy, RootWindow(dpy, 0), DefaultVisual(dpy, 0),
                         AllocNone);

  XAllocNamedColor(dpy, cmap, "black", &black, &blackTrue);
  XAllocNamedColor(dpy, cmap, "white", &white, &whiteTrue);

  root = XCreateSimpleWindow(dpy, RootWindow(dpy,0), 0, 0, FieldW+20,
                             FieldH+20, 0, white.pixel, black.pixel);

  XSetWindowColormap(dpy, root, cmap);

  XSelectInput(dpy, root, ExposureMask|EnterWindowMask|LeaveWindowMask|KeyPressMask|KeyReleaseMask);
  XStoreName(dpy, root, "xsoldier");
  sh.flags = (PMaxSize | PMinSize);
  sh.min_width = FieldW + 20;
  sh.min_height = FieldH + 20;
  sh.max_width = FieldW + 20;
  sh.max_height = FieldH + 20;
  XSetWMNormalHints(dpy, root, &sh);

  win = XCreateSimpleWindow(dpy, root, 10, 10, FieldW, FieldH, 1,
                            white.pixel, black.pixel);
  XSelectInput(dpy, win, ExposureMask|EnterWindowMask|KeyPressMask|KeyReleaseMask);

  WorkPixmap = XCreatePixmap(dpy, win, FieldW, FieldH, DefaultDepth(dpy, 0));
  FontGC     = XCreateGC(dpy,root,0,0);
  XSetGraphicsExposures(dpy,FontGC,False);

  BackGC       = XCreateGC(dpy,WorkPixmap,0,0);
  XSetGraphicsExposures(dpy,BackGC,False);
  XSetForeground(dpy,BackGC,black.pixel);

  FillGC       = XCreateGC(dpy,WorkPixmap,0,0);
  XSetGraphicsExposures(dpy,FillGC,False);
  XSetForeground(dpy,FillGC,white.pixel);


  PlayerImage = ImageInit( "Player.xpm",6);
  PShot1Image  = ImageInit( "PlayerShot1.xpm",2);
  PShot2Image  = ImageInit( "PlayerShot2.xpm",2);
  PShot3Image  = ImageInit( "PlayerShot3.xpm",3);

  EShotImage  = ImageInit( "EnemyShot.xpm",4);
  ELaserImage  = ImageInit( "EnemyLaser.xpm",1);
  EMissileImage  = ImageInit( "EnemyMiss.xpm",8);
  EBoundImage = ImageInit( "EnemyBound.xpm",8);
  ERingImage = ImageInit( "EnemyRing.xpm",4);

  BombImage   = ImageInit( "ExpSmall.xpm",5);
  LargeBombImage= ImageInit( "ExpLarge.xpm",5);

  Enemy1Image = ImageInit( "Enemy1.xpm",8);
  Enemy2Image = ImageInit( "Enemy2.xpm",8);
  Enemy3Image = ImageInit( "Enemy3.xpm",8);
  Enemy4Image = ImageInit( "Enemy4.xpm",8);
  Enemy5Image = ImageInit( "Enemy5.xpm",4);
  Enemy6Image = ImageInit( "Enemy6.xpm",6);
  Enemy7Image = ImageInit( "Enemy7.xpm",1);

  Boss1Image = ImageInit( "Boss1.xpm",1);
  Boss2Image = ImageInit( "Boss2.xpm",1);
  Boss3Image = ImageInit( "Boss3.xpm",1);
  Boss4Image = ImageInit( "Boss4.xpm",1);
  Boss5Image = ImageInit( "Boss5.xpm",1);
  Boss6Image = ImageInit( "Boss6.xpm",2);
  Boss7Image = ImageInit( "Boss7.xpm",1);

  ItemImage = ImageInit( "Item.xpm",4);
    
  /* initialize font */
  /* explanation of font images
   *  14 * 7
   *  0@P`p 14
   * !1AQaq 28
   * "2BRbr 42
   * #3CScs 56
   * $4DTdt 70
   * %5EUeu 84
   * &6FVfv 98
   * '7GWgw 112
   * (8HXhx 126
   * )9IYiy 140
   * *:JZjz 154
   * +;K[k{ 168
   * ,<L\l| 182
   * -=M]m} 196
   * .>N^n~ 210
   * /?O_o  224
   */
  Font1Image = ImageInit( "font1.xpm" , 16);
  Font2Image = ImageInit( "font2.xpm" , 16);
  Font3Image = ImageInit( "font3.xpm" , 16);
  Font4Image = ImageInit( "font4.xpm" , 16);
  Font5Image = ImageInit( "font5.xpm" , 16);
  Font6Image = ImageInit( "font6.xpm" , 16);

  return 0;
}

int clear_window(void) {
  XFillRectangle(dpy, WorkPixmap, BackGC, 0, 0, FieldW, FieldH);
  return 0;
}


int redraw_window(void) {
  XCopyArea(dpy,WorkPixmap,win,BackGC,0,0,FieldW,FieldH,0,0);
  XFlush(dpy);
  XSync(dpy,False);

  XImage* image = XGetImage(dpy,WorkPixmap,0,0,FieldW,FieldH,AllPlanes,ZPixmap);
  for(int row=0;row<FieldH;row++){
    for(int col=0;col<FieldW;col++){
      printf( "%06lx", XGetPixel(image,col,row) );
    }
  }
  printf("\n");

  return 0;
}

int graphic_finish(void) {
  FreeImages(PlayerImage,6);
  FreeImages(PShot1Image,2);
  FreeImages(PShot2Image,2);
  FreeImages(PShot3Image,3);

  FreeImages(EShotImage,4);
  FreeImages(ELaserImage,1);
  FreeImages(EMissileImage,8);
  FreeImages(EBoundImage,8);
  FreeImages(ERingImage,4);

  FreeImages(BombImage,5);
  FreeImages(LargeBombImage,5);

  FreeImages(Enemy1Image,8);
  FreeImages(Enemy2Image,8);
  FreeImages(Enemy3Image,8);
  FreeImages(Enemy4Image,8);
  FreeImages(Enemy5Image,4);
  FreeImages(Enemy6Image,6);
  FreeImages(Enemy7Image,1);

  FreeImages(Boss1Image,1);
  FreeImages(Boss2Image,1);
  FreeImages(Boss3Image,1);
  FreeImages(Boss4Image,1);
  FreeImages(Boss5Image,1);
  FreeImages(Boss6Image,2);
  FreeImages(Boss7Image,1);

  FreeImages(ItemImage,4);

  FreeImages(Font1Image, 16);
  FreeImages(Font2Image, 16);
  FreeImages(Font3Image, 16);
  FreeImages(Font4Image, 16);
  FreeImages(Font5Image, 16);
  FreeImages(Font6Image, 16);

  XFreeGC(dpy, FontGC);
  XFreeGC(dpy, BackGC);
  XFreeGC(dpy, FillGC);
  XAutoRepeatOn(dpy);
  XFlush(dpy);
  XCloseDisplay(dpy);
  return 0;
}


int draw_string(int x, int y, const char *string, int length) {
  int i;
  y -= 7;
  for (i = 0; (i < length) && (string[i] != '\0'); i++)
  {
    draw_char(x, y, string[i]);
    x += 7;
  }
  
  return 0;
}

/* return 0 on success, negative value on error */
int draw_char(int x, int y, int c) {
  if (!isprint(c)) { c = '?'; }

  /* don't assume ASCII */
  switch ( c ) {
  case ' ':
    /* do nothing */
    return 0;
  case '!':
    PutImage(Font1Image[1], x, y);
    return 0;
  case '"':
    PutImage(Font1Image[2], x, y);
    return 0;
  case '#':
    PutImage(Font1Image[3], x, y);
    return 0;
  case '$':
    PutImage(Font1Image[4], x, y);
    return 0;
  case '%':
    PutImage(Font1Image[5], x, y);
    return 0;
  case '&':
    PutImage(Font1Image[6], x, y);
    return 0;
  case '\'':
    PutImage(Font1Image[7], x, y);
    return 0;
  case '(':
    PutImage(Font1Image[8], x, y);
    return 0;
  case ')':
    PutImage(Font1Image[9], x, y);
    return 0;
  case '*':
    PutImage(Font1Image[10], x, y);
    return 0;
  case '+':
    PutImage(Font1Image[11], x, y);
    return 0;
  case ',':
    PutImage(Font1Image[12], x, y);
    return 0;
  case '-':
    PutImage(Font1Image[13], x, y);
    return 0;
  case '.':
    PutImage(Font1Image[14], x, y);
    return 0;
  case '/':
    PutImage(Font1Image[15], x, y);
    return 0;
  case '0':
    PutImage(Font2Image[0], x, y);
    return 0;
  case '1':
    PutImage(Font2Image[1], x, y);
    return 0;
  case '2':
    PutImage(Font2Image[2], x, y);
    return 0;
  case '3':
    PutImage(Font2Image[3], x, y);
    return 0;
  case '4':
    PutImage(Font2Image[4], x, y);
    return 0;
  case '5':
    PutImage(Font2Image[5], x, y);
    return 0;
  case '6':
    PutImage(Font2Image[6], x, y);
    return 0;
  case '7':
    PutImage(Font2Image[7], x, y);
    return 0;
  case '8':
    PutImage(Font2Image[8], x, y);
    return 0;
  case '9':
    PutImage(Font2Image[9], x, y);
    return 0;
  case ':':
    PutImage(Font2Image[10], x, y);
    return 0;
  case ';':
    PutImage(Font2Image[11], x, y);
    return 0;
  case '<':
    PutImage(Font2Image[12], x, y);
    return 0;
  case '=':
    PutImage(Font2Image[13], x, y);
    return 0;
  case '>':
    PutImage(Font2Image[14], x, y);
    return 0;
  case '?':
    PutImage(Font2Image[15], x, y);
    return 0;
  case '@':
    PutImage(Font3Image[0], x, y);
    return 0;
  case 'A':
    PutImage(Font3Image[1], x, y);
    return 0;
  case 'B':
    PutImage(Font3Image[2], x, y);
    return 0;
  case 'C':
    PutImage(Font3Image[3], x, y);
    return 0;
  case 'D':
    PutImage(Font3Image[4], x, y);
    return 0;
  case 'E':
    PutImage(Font3Image[5], x, y);
    return 0;
  case 'F':
    PutImage(Font3Image[6], x, y);
    return 0;
  case 'G':
    PutImage(Font3Image[7], x, y);
    return 0;
  case 'H':
    PutImage(Font3Image[8], x, y);
    return 0;
  case 'I':
    PutImage(Font3Image[9], x, y);
    return 0;
  case 'J':
    PutImage(Font3Image[10], x, y);
    return 0;
  case 'K':
    PutImage(Font3Image[11], x, y);
    return 0;
  case 'L':
    PutImage(Font3Image[12], x, y);
    return 0;
  case 'M':
    PutImage(Font3Image[13], x, y);
    return 0;
  case 'N':
    PutImage(Font3Image[14], x, y);
    return 0;
  case 'O':
    PutImage(Font3Image[15], x, y);
    return 0;
  case 'P':
    PutImage(Font4Image[0], x, y);
    return 0;
  case 'Q':
    PutImage(Font4Image[1], x, y);
    return 0;
  case 'R':
    PutImage(Font4Image[2], x, y);
    return 0;
  case 'S':
    PutImage(Font4Image[3], x, y);
    return 0;
  case 'T':
    PutImage(Font4Image[4], x, y);
    return 0;
  case 'U':
    PutImage(Font4Image[5], x, y);
    return 0;
  case 'V':
    PutImage(Font4Image[6], x, y);
    return 0;
  case 'W':
    PutImage(Font4Image[7], x, y);
    return 0;
  case 'X':
    PutImage(Font4Image[8], x, y);
    return 0;
  case 'Y':
    PutImage(Font4Image[9], x, y);
    return 0;
  case 'Z':
    PutImage(Font4Image[10], x, y);
    return 0;
  case '[':
    PutImage(Font4Image[11], x, y);
    return 0;
  case '\\':
    PutImage(Font4Image[12], x, y);
    return 0;
  case ']':
    PutImage(Font4Image[13], x, y);
    return 0;
  case '^':
    PutImage(Font4Image[14], x, y);
    return 0;
  case '_':
    PutImage(Font4Image[15], x, y);
    return 0;
  case '`':
    PutImage(Font5Image[0], x, y);
    return 0;
  case 'a':
    PutImage(Font5Image[1], x, y);
    return 0;
  case 'b':
    PutImage(Font5Image[2], x, y);
    return 0;
  case 'c':
    PutImage(Font5Image[3], x, y);
    return 0;
  case 'd':
    PutImage(Font5Image[4], x, y);
    return 0;
  case 'e':
    PutImage(Font5Image[5], x, y);
    return 0;
  case 'f':
    PutImage(Font5Image[6], x, y);
    return 0;
  case 'g':
    PutImage(Font5Image[7], x, y);
    return 0;
  case 'h':
    PutImage(Font5Image[8], x, y);
    return 0;
  case 'i':
    PutImage(Font5Image[9], x, y);
    return 0;
  case 'j':
    PutImage(Font5Image[10], x, y);
    return 0;
  case 'k':
    PutImage(Font5Image[11], x, y);
    return 0;
  case 'l':
    PutImage(Font5Image[12], x, y);
    return 0;
  case 'm':
    PutImage(Font5Image[13], x, y);
    return 0;
  case 'n':
    PutImage(Font5Image[14], x, y);
    return 0;
  case 'o':
    PutImage(Font5Image[15], x, y);
    return 0;
  case 'p':
    PutImage(Font6Image[0], x, y);
    return 0;
  case 'q':
    PutImage(Font6Image[1], x, y);
    return 0;
  case 'r':
    PutImage(Font6Image[2], x, y);
    return 0;
  case 's':
    PutImage(Font6Image[3], x, y);
    return 0;
  case 't':
    PutImage(Font6Image[4], x, y);
    return 0;
  case 'u':
    PutImage(Font6Image[5], x, y);
    return 0;
  case 'v':
    PutImage(Font6Image[6], x, y);
    return 0;
  case 'w':
    PutImage(Font6Image[7], x, y);
    return 0;
  case 'x':
    PutImage(Font6Image[8], x, y);
    return 0;
  case 'y':
    PutImage(Font6Image[9], x, y);
    return 0;
  case 'z':
    PutImage(Font6Image[10], x, y);
    return 0;
  case '{':
    PutImage(Font6Image[11], x, y);
    return 0;
  case '|':
    PutImage(Font6Image[12], x, y);
    return 0;
  case '}':
    PutImage(Font6Image[13], x, y);
    return 0;
  case '~':
    PutImage(Font6Image[14], x, y);
    return 0;
  default:
    fprintf(stderr, "draw_char: unknown char found (\\x%x), ignoring", c);
    return -1;
  }
  /* should not reach here */
  return -2;
}

