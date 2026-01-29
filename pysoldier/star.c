/* xsoldier, a shoot 'em up game with "not shooting" bonus
 * Copyright (C) 1997 Yuusuke HASHIMOTO <s945750@educ.info.kanagawa-u.ac.jp>
 * Copyright (C) 2002 Oohara Yuuma  <oohara@libra.interq.or.jp>
 *
 * This is a copyleft program.  See the file LICENSE for details.
 */
/* $Id: star.c,v 1.8 2002/05/06 04:26:20 oohara Exp $ */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
/*
#include <X11/Xlib.h>
*/

#include "image.h"
#include "xsoldier.h"
#include "extern.h"
#include "star.h"

#define MAXSTAR 4

typedef struct {
  /* coordinates of the star */
    int x, y;
  /* pattern number of the star */
    int cpat;
} Point;

typedef struct {
    Point *point;
    Image **image;
    unsigned int sW, sH;

    int pattern;
    int speed;
    int nstar;
} Star;

static Star     star[MAXSTAR];
static int      cust;
static unsigned int dW, dH;

void InitStarModule(unsigned int w, unsigned int h)
{
    dW = w; dH = h;
    cust = 0;
    return;
}

int CreateStar(const char *filename, int pattern, int speed, int nstar) {
    return 0;
}

void DrawStar(int id)
{
    static Image *I;
    static Star *S;
    static Point *p;
    int i;


    S = &(star[id]);
    for (i=0; i<S->nstar; i++)
    {
	p = &(S->point[i]);
	I = S->image[(p->cpat)++];
#ifdef HAVE_LIBSDL
	PutImage(I,p->x - I->w, p->y - I->h);
#else /* not HAVE_LIBSDL */
	PutImage(I,p->x - I->width, p->y - I->height);
#endif /* not HAVE_LIBSDL */
	if (p->cpat >= S->pattern) p->cpat = 0;
    }
    
    for (i=0; i<S->nstar; i++)
    {
	p = &(S->point[i]);
	p->y += S->speed;
	if (p->y < 0 || p->y > S->sH)
	{
	    p->x = integerrng() % S->sW;
	    p->y = (S->speed > 0)?0:S->sH;
	}
    }

    return;
}

void ChangeStarParameter(int id, int speed)
{
    static Star *S;

    S = &(star[id]);
    S->speed     = speed;
    return;
}

int DeleteAllStar(void)
{
  int i;
  for (i = 0; i < cust; i++)
  {
    FreeImages(star[i].image, star[i].pattern);
    free(star[i].point);
  }
  
  return 0;
}
