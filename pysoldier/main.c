/* xsoldier, a shoot 'em up game with "not shooting" bonus
 * Copyright (C) 1997 Yuusuke HASHIMOTO <s945750@educ.info.kanagawa-u.ac.jp>
 * Copyright (C) 2002 Oohara Yuuma  <oohara@libra.interq.or.jp>
 *
 * This is a copyleft program.  See the file LICENSE for details.
 */
/* $Id: main.c,v 1.31 2006/09/16 09:20:54 oohara Exp $ */

/* DEBUG and JSTK are defined in config.h */
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

/* local functions */

static void init(void);
static void clean(void);
static void sig_handle(int arg);

/* file-global variables */
static int w_time = -1;

int main(int argc, char *argv[]) {
  display[0] = '\0';
  manage = NULL;
  player = NewPlayerData();
  init();
  manage = NewManage(256,256);
  manage->start_power = 0;
    
#ifndef HAVE_LIBSDL
    XMapWindow(dpy,win);
    XMapWindow(dpy,root);

    XFlush(dpy);
#endif /* not HAVE_LIBSDL */

	keymask = 0;
	player->Rec[0].score = 0;
	player->Ships = 5;
	player->Next = FIRST1UP;
        

	while (1) {
	    keymask = 0;
	    ResetManage(manage);
	    if (mainLoop() == 0){ break;}
	}


    DeleteManage(manage);
    free(player);
    clean();
    return 0;
}



static void init(void) {
    struct itimerval value, ovalue;
    struct sigaction sig_act;

    /* set wait */
    signal_delivered = 1;
    if (w_time < 0)
	waittime = WAIT;
    else
	waittime = w_time;

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

    /* you must call graphic_init() first because it calls SDL_Init */
    graphic_init();
    input_init();
    srand48(1234);
}

static void clean(void) { graphic_finish(); }
static void sig_handle(int arg) { signal_delivered = 1; }
