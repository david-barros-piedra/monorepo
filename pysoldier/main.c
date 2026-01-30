#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "xsoldier.h"
#include "manage.h"
#include "game.h"
#include "player.h"
#include "wait.h"
#include "graphic.h"
#include "input.h"

/* define all "extern" here */
#define EXTERN_DEF
#include "extern.h"

static void init(void) {
  graphic_init();
  input_init();
  srand48(1234);
}

int main(int argc, char *argv[]) {
  manage = NULL;
  player = NewPlayerData();
  init();
  manage = NewManage(256,256);
  manage->start_power = 0;

  XMapWindow(dpy,win);
  XMapWindow(dpy,root);
  XFlush(dpy);

  keymask = 0;
  player->Rec[0].score = 0;
  player->Ships = 99;
  player->Next = FIRST1UP;
  while (1) {
    keymask = 0;
    ResetManage(manage);
    if (mainLoop() == 1){ break;}
  }
  DeleteManage( manage );
  free(player);
  graphic_finish();
  return 0;
}
