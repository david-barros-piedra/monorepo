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


static void sig_handle(int arg) { signal_delivered = 1; }

static void init(void) {
    struct itimerval value, ovalue;
    struct sigaction sig_act;

    /* set wait */
    signal_delivered = 1;

    waittime = 35000;

    memset(&sig_act, 0, sizeof(sig_act));
    sig_act.sa_handler = sig_handle;
    sigaction(SIGALRM, &sig_act, 0);
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = waittime;
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = waittime;
    setitimer(ITIMER_REAL, &value, &ovalue);

    FieldW  = 500;
    FieldH  = 650;

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
