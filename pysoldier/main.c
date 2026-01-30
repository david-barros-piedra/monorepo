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
/* xsoldier, a shoot 'em up game with "not shooting" bonus
 * Copyright (C) 1997 Yuusuke HASHIMOTO <s945750@educ.info.kanagawa-u.ac.jp>
 * Copyright (C) 2002 Oohara Yuuma  <oohara@libra.interq.or.jp>
 *
 * This is a copyleft program.  See the file LICENSE for details.
 */
/* $Id: game.c,v 1.34 2009/11/08 06:21:24 oohara Exp $ */

/* DEBUG and JSTK are defined in config.h */
#include <config.h>
/* pause */
#include <unistd.h>
/* rand */
#include <stdlib.h>
/* strlen */
#include <string.h>

#include <stdio.h>
/*
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
*/

#include "image.h"
#include "xsoldier.h"
#include "player.h"







extern int NewBoss1(void);
extern DelAtt BossAct1(ObjData *my);
extern DelAtt BossHit1(ObjData *my, ObjData *your);

extern int NewBoss2(void);
extern DelAtt BossAct2(ObjData *my);

extern int NewBoss3(void);
extern DelAtt BossAct3(ObjData *my);

extern int NewBoss4(void);
extern DelAtt BossAct4(ObjData *my);

extern int NewBoss5(void);
extern DelAtt BossAct5(ObjData *my);

extern int NewBoss6(void);
extern DelAtt BossAct6(ObjData *my);

extern int NewBoss7(void);
extern DelAtt BossAct7(ObjData *my);

extern int NewBoss8(void);
extern DelAtt BossAct8(ObjData *my);
extern DelAtt BossHit8(ObjData *my, ObjData *your);



/* DEBUG is defined in config.h */
#include <config.h>
/* rand */
#include <stdlib.h>

/*
#include <X11/Xlib.h>
#include <X11/xpm.h>
*/

#include "image.h"
#include "xsoldier.h"
#include "manage.h"
#include "common.h"
#include "enemyshot.h"
#include "callback.h"
#include "enemy.h"
#include "extern.h"

/* local functions for the last boss */
static DelAtt BossAct8_dead(ObjData *my);
static DelAtt BossAct8_rand_shot(ObjData *my);
static DelAtt BossAct8_ring_shot(ObjData *my);
static DelAtt BossAct8_bound_shot(ObjData *my);
static DelAtt BossAct8_homing_shot(ObjData *my);
static DelAtt BossAct8_charge_shot(ObjData *my);
static DelAtt BossAct8_n_way_shot(ObjData *my);
static void BossAct8_next(ObjData *my, int span);

/* definition of objects
 * - initialization function
 * - action function
 * - hit function
 *
 * initialization add action function and display/hit function to the table
 */

int NewBoss1(void)
{
    manage->New.Data.hitAtt = MEnemy;
    manage->New.Data.hitMask = MPlayer | MPShot;

    manage->New.Data.X = FieldW/2;
    manage->New.Data.Y = -90;

    manage->New.Data.HP = 120;
    manage->New.Data.Point = 5000;
    manage->New.Data.EnemyAtt = BossDel;

    manage->New.Data.Width = 150;
    manage->New.Data.Height = 180;

    manage->New.Data.Cnt[0] = 0;
    manage->New.Data.Cnt[1] = 200; /* immutable time */
    manage->New.Data.Cnt[2] = 0; /* image counter*/
    manage->New.Data.Cnt[3] = 10; /* x move */
    manage->New.Data.Cnt[4] = 0; /* y move */
    manage->New.Data.Cnt[5] = 0;  /* laser counter */
    manage->New.Data.Cnt[6] = 0;  /* normal shot counter */

    manage->New.Grp.image = Boss1Image;
    return NewObj(MEnemy,BossAct1,BossHit1,DrawImage);
}

DelAtt BossAct1(ObjData *my)
{
  int i;
  
    if (my->Cnt[0] < my->Cnt[1])
    {/* appears slowly */
	my->Y++;
	my->Cnt[0]++;
	return NoneDel;
    }

    if (manage->BossTime <= 0)
    {
      if (my->Y > 0 - my->Height/2 - 100)
        (my->Y) -= 4;
      if (my->Y < 0 - my->Height/2 - 50)
      {
        if ((player->Ships <= 0) && (manage->player[0]->Data.Cnt[3] != 0))
          return NoneDel;
        else
          return BossDel;
      }
      else
	return NoneDel;
    }

    (manage->BossTime)--;

    if (my->Cnt[6] >= (100- (manage->Loop)*10) )
    {
        ShotToAngle(my->X, my->Y, 50, 10);
        ShotToAngle(my->X, my->Y, 70, 10);
        ShotToAngle(my->X, my->Y, 90, 10);
        ShotToAngle(my->X, my->Y, 110, 10);
        ShotToAngle(my->X, my->Y, 130, 10);
        my->Cnt[6] =0;
    }
    else
	(my->Cnt[6])++;

    if (my->Cnt[5] >= (25-manage->Loop*3))
    {
      for (i = 1; i< manage->Loop; i++)
	    ShotToAngle(my->X,my->Y,integerrng()%180,10);
      if (manage->Loop >1)
	    ShotToPoint(my->X,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y,10);
	LaserShot(my->X-30,my->Y,20);
	LaserShot(my->X+30,my->Y,20);
	LaserShot(my->X-80,my->Y,20);
	LaserShot(my->X+80,my->Y,20);

	my->Cnt[5] = 0;
    }
    else
	my->Cnt[5]++;

    if ((my->X+my->Cnt[3]>FieldW) || (my->X+my->Cnt[3]<0))
	my->Cnt[3] = my->Cnt[3]*(-1);

    my->X += my->Cnt[3];

    return NoneDel;
}

DelAtt BossHit1(ObjData *my, ObjData *your)
{
  int temp = your->Attack;
  
  if ((my->Cnt[0] < my->Cnt[1]) || (manage->BossTime <=0))
    {
      return NoneDel;
    }
    
    if (my->HP < temp)
    {
      temp = my->HP;
    }
    
    my->HP -= temp;
    player->Rec[0].score += temp;
    
    if (my->HP <= 0)
    {
      player->Rec[0].score -= 1;
        NewBomb(my->X,my->Y);
        NewBomb(my->X+integerrng()%100-50,my->Y+integerrng()%100-50);
        NewBomb(my->X+integerrng()%100-50,my->Y+integerrng()%100-50);
        NewBomb(my->X+integerrng()%100-50,my->Y+integerrng()%100-50);
        NewBomb(my->X+integerrng()%100-50,my->Y+integerrng()%100-50);
        NewBomb(my->X+integerrng()%100-50,my->Y+integerrng()%100-50);
        NewBomb(my->X+integerrng()%100-50,my->Y+integerrng()%100-50);

        my->showDamegeTime = 0;
        return my->EnemyAtt;
    }
    else
    {
      my->showDamegeTime = 15;
      return NoneDel;
    }
    
}

int NewBoss2(void)
{
    manage->New.Data.hitAtt = MEnemy;
    manage->New.Data.hitMask = MPlayer | MPShot;

    manage->New.Data.X = FieldW/2;
    manage->New.Data.Y = -60;

    manage->New.Data.HP = 200;
    manage->New.Data.Point = 5000;
    manage->New.Data.EnemyAtt = BossDel;

    manage->New.Data.Width = 90;
    manage->New.Data.Height = 130;

    manage->New.Data.Cnt[0] = 0;
    manage->New.Data.Cnt[1] = 200; /* immutable time */
    manage->New.Data.Cnt[2] = 0; /* image counter */

    manage->New.Data.Cnt[3] = 0;
    manage->New.Data.Cnt[4] = 0;
    manage->New.Data.Cnt[5] = 0; /* normal shot counter */
    
    manage->New.Grp.image = Boss2Image;
    return NewObj(MEnemy,BossAct2,BossHit1,DrawImage);
}

DelAtt BossAct2(ObjData *my)
{
    if (my->Cnt[0] < my->Cnt[1])
    {/* appears slowly */
	my->Y++;
	my->Cnt[0]++;
	return NoneDel;
    }

    if (manage->BossTime <= 0)
    {
      if (my->Y > 0 - my->Height/2 - 100)
        (my->Y) -= 4;
      if (my->Y < 0 - my->Height/2 - 50)
      {
        if ((player->Ships <= 0) && (manage->player[0]->Data.Cnt[3] != 0))
          return NoneDel;
        else
          return BossDel;
      }
      else
	return NoneDel;
    }

    (manage->BossTime)--;

    if (manage->Loop > 2)
    {
	if (my->Cnt[5] >= 90- (manage->Loop)*10)
	{
	    ShotToPoint(my->X+40,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y,integerrng()%3 + 5);
	    ShotToPoint(my->X   ,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y,8);
	    ShotToPoint(my->X-40,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y,integerrng()%5 + 3);
            my->Cnt[5] = 0;
	}
	else
            (my->Cnt[5])++;
    }

    if (my->Cnt[3] >= (15-manage->Loop*3))
    {
      if (manage->Loop > 1)
      {
        if (manage->player[0]->Data.Y < my->Y)
          ShotToAngle(my->X, my->Y, 270, 10);
        else
          ShotToAngle(my->X, my->Y, 90, 10);
      }
      
      HomingShot(my->X-20,my->Y-10,-10,-10);
      HomingShot(my->X+20,my->Y-10, 10,-10);
      my->Cnt[3] = 0;
    }
    else
	my->Cnt[3]++;

    if (my->X > manage->player[0]->Data.X)
    {
	if (my->X - manage->player[0]->Data.X > 20)
	{
	    my->X -= 10;
	}
	else
	{
	    my->Cnt[4]++;
	    my->X --;
	}

	if (my->Cnt[4] > 10)
	{
	    LaserShot(my->X-40,my->Y,30);
	    LaserShot(my->X+40,my->Y,30);
	    my->Cnt[4] = 0;
	}
    }
    else
    {
	if (manage->player[0]->Data.X - my->X > 20)
	{
	    my->X += 10;
	}
	else
	{
	    my->Cnt[4]++;
	    my->X ++;
	}

	if (my->Cnt[4] > 10)
	{
	    LaserShot(my->X-40,my->Y,30);
	    LaserShot(my->X+40,my->Y,30);
	    my->Cnt[4] = 0;
	}
    }

    return NoneDel;
}

int NewBoss3(void)
{
    manage->New.Data.hitAtt = MEnemy;
    manage->New.Data.hitMask = MPlayer | MPShot;

    manage->New.Data.X = FieldW/2;
    manage->New.Data.Y = -80;

    manage->New.Data.HP = 330;
    manage->New.Data.Point = 6000;
    manage->New.Data.EnemyAtt = BossDel;

    manage->New.Data.Width = 130;
    manage->New.Data.Height = 130;

    manage->New.Data.Cnt[0] = 0;
    manage->New.Data.Cnt[1] = 200; /* immutable time */
    manage->New.Data.Cnt[2] = 0; /* image counter */
    manage->New.Data.Cnt[3] = 10; /* x move */
    manage->New.Data.Cnt[4] = 0; /* y move */
    manage->New.Data.Cnt[5] = 0;  /* laser counter */
    manage->New.Data.Cnt[6] = 0; /* missile counter */

    manage->New.Grp.image = Boss3Image;
    return NewObj(MEnemy,BossAct3,BossHit1,DrawImage);
}

DelAtt BossAct3(ObjData *my)
{
    if (my->Cnt[0] < my->Cnt[1])
    {/* appears slowly */
	my->Y++;
	my->Cnt[0]++;
	return NoneDel;
    }

    if (manage->BossTime <= 0)
    {
      if (my->Y > 0 - my->Height/2 - 100)
        (my->Y) -= 4;
      if (my->Y < 0 - my->Height/2 - 50)
      {
        if ((player->Ships <= 0) && (manage->player[0]->Data.Cnt[3] != 0))
          return NoneDel;
        else
          return BossDel;
      }
      else
	return NoneDel;
    }

    (manage->BossTime)--;

    if (my->Cnt[6] >= (80 - manage->Loop*10))
    {
	HomingShot(my->X,my->Y,0,10);
	HomingShot(my->X,my->Y,10,0);
	HomingShot(my->X,my->Y,0,-10);
	HomingShot(my->X,my->Y,-10,0);
	HomingShot(my->X,my->Y,10,10);
	HomingShot(my->X,my->Y,-10,10);
	HomingShot(my->X,my->Y,10,-10);
	HomingShot(my->X,my->Y,-10,-10);

	my->Cnt[6] = 0;
    }
    else
      my->Cnt[6]++;

    if (my->Cnt[5] >= (25 - manage->Loop*2))
    {
      if (manage->Loop >1)
	LaserShot(my->X- 120+ (integerrng()%5)*60,my->Y,25);
      if (manage->Loop >2)
	    ShotToPoint(my->X,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y,18);
	LaserShot(my->X-30,my->Y,25);
	LaserShot(my->X+30,my->Y,25);
	LaserShot(my->X-90,my->Y,25);
	LaserShot(my->X+90,my->Y,25);
	LaserShot(my->X-150,my->Y,25);
	LaserShot(my->X+150,my->Y,25);

	my->Cnt[5] = 0;
    }
    else
	my->Cnt[5]++;

    if ((my->X+my->Cnt[3]>FieldW) || (my->X+my->Cnt[3]<0))
	my->Cnt[3] = my->Cnt[3]*(-1);

    my->X += my->Cnt[3];

    return NoneDel;
}

int NewBoss4(void)
{
    manage->New.Data.hitAtt = MEnemy;
    manage->New.Data.hitMask = MPlayer | MPShot;

    manage->New.Data.X = FieldW/2;
    manage->New.Data.Y = -90;

    manage->New.Data.inertX = 0;
    manage->New.Data.inertY = 0;

    manage->New.Data.HP = 550;
    manage->New.Data.Point = 6000;
    manage->New.Data.EnemyAtt = BossDel;

    manage->New.Data.Width = 300;
    manage->New.Data.Height = 170;

    manage->New.Data.Cnt[0] = 0;
    manage->New.Data.Cnt[1] = 195; /* immutable time */
    manage->New.Data.Cnt[2] = 0; /* image counter */
    manage->New.Data.Cnt[3] = 1; /* x move */
    manage->New.Data.Cnt[4] = 0; /* y move */
    manage->New.Data.Cnt[5] = 0;  /* laser counter */

    manage->New.Grp.image = Boss4Image;
    return NewObj(MEnemy,BossAct4,BossHit1,DrawImage);
}

DelAtt BossAct4(ObjData *my)
{
    if (my->Cnt[0] < my->Cnt[1])
    {/* appears slowly */
	my->Y++;
	my->Cnt[0]++;
	return NoneDel;
    }

    if (manage->BossTime <= 0)
    {
      if (my->Y > 0 - my->Height/2 - 100)
        (my->Y) -= 4;
      if (my->Y < 0 - my->Height/2 - 50)
      {
        if ((player->Ships <= 0) && (manage->player[0]->Data.Cnt[3] != 0))
          return NoneDel;
        else
          return BossDel;
      }
      else
	return NoneDel;
    }

    (manage->BossTime)--;

    if (my->Cnt[5] >= (8-manage->Loop))
    {
	LaserShot(my->X + integerrng()%600-300, my->Y, 11 + integerrng()%3  + manage->Loop*7);
	LaserShot(my->X + integerrng()%600-300, my->Y, 6 + integerrng()%3 + manage->Loop*7);
	LaserShot(my->X + integerrng()%600-300, my->Y, 1 + integerrng()%3 + manage->Loop*7);
	LaserShot(my->X + integerrng()%600-300, my->Y, -4 + integerrng()%3 + manage->Loop*7);
        if (manage->Loop >2)
	LaserShot(my->X + integerrng()%600-300, my->Y, 6 + integerrng()%3 + manage->Loop*7);
        if (manage->Loop >1)
	LaserShot(my->X + integerrng()%600-300, my->Y, 1 + integerrng()%3 + manage->Loop*7);
	my->Cnt[5] = 0;
    }
    else
	my->Cnt[5]++;

    if (my->X < manage->player[0]->Data.X)
    {
        if (my->inertX <= 8)
            my->inertX++;
    }
    if (my->X > manage->player[0]->Data.X)
    {
        if (my->inertX >= -8)
            my->inertX--;
    }

    my->X += my->inertX;

    return NoneDel;
}

int NewBoss5(void)
{
    manage->New.Data.hitAtt = MEnemy;
    manage->New.Data.hitMask = MPlayer | MPShot;

    manage->New.Data.X = FieldW/2;
    manage->New.Data.Y = -128;

    manage->New.Data.HP = 350;
    manage->New.Data.Point = 7000;
    manage->New.Data.EnemyAtt = BossDel;

    manage->New.Data.Width = 120;
    manage->New.Data.Height = 120;

    manage->New.Data.Cnt[0] = 0;
    manage->New.Data.Cnt[1] = 200; /* immutable time */
    manage->New.Data.Cnt[2] = 0; /* image counter */
    manage->New.Data.Cnt[3] = manage->player[0]->Data.X; /* x move */
    manage->New.Data.Cnt[4] = manage->player[0]->Data.Y; /* y move */
    manage->New.Data.Cnt[5] = 0;  /* shot counter */
    manage->New.Data.Cnt[6] = 0;  /* angle counter */

    manage->New.Grp.image = Boss5Image;
    return NewObj(MEnemy,BossAct5,BossHit1,DrawImage);
}

DelAtt BossAct5(ObjData *my)
{
  int i;

    if (my->Cnt[0] < my->Cnt[1])
    {/* appears slowly */
	my->Y++;
	my->Cnt[0]++;
	return NoneDel;
    }

    if (manage->BossTime <= 0)
    {
      if (my->Y > 0 - my->Height/2 - 100)
        (my->Y) -= 4;
      if (my->Y < 0 - my->Height/2 - 50)
      {
        if ((player->Ships <= 0) && (manage->player[0]->Data.Cnt[3] != 0))
          return NoneDel;
        else
          return BossDel;
      }
      else
	return NoneDel;
    }

    (manage->BossTime)--;

    if (my->Cnt[5] > 120)
    {
	my->Cnt[3] = manage->player[0]->Data.X;
	my->Cnt[4] = manage->player[0]->Data.Y;
	my->Cnt[5] = 0;
    }
    else if (abs(my->X-my->Cnt[3]) > 5 || abs(my->Y-my->Cnt[4]) > 5)
    {
	if (my->X < my->Cnt[3])
	{
	    if (my->Cnt[3]-my->X > 50)
		my->X += 20;
	    else if (my->Cnt[3]-my->X > 30)
		my->X += 10;
	    else if (my->Cnt[3]-my->X > 20)
		my->X += 5;
	    else
		my->X ++;
	}
	else
	{
	    if (my->X-my->Cnt[3] > 50)
		my->X -= 20;
	    else if (my->X-my->Cnt[3] > 30)
		my->X -= 10;
	    else if (my->X-my->Cnt[3] > 20)
		my->X -= 5;
	    else
		my->X --;
	}

	if (my->Y < my->Cnt[4])
	{
	    if (my->Cnt[4]-my->Y > 50)
		my->Y += 20;
	    else if (my->Cnt[4]-my->Y > 30)
		my->Y += 10;
	    else if (my->Cnt[4]-my->Y > 20)
		my->Y += 5;
	    else
		my->Y ++;
	}
	else
	{
	    if (my->Y-my->Cnt[4] > 50)
		my->Y -= 20;
	    else if (my->Y-my->Cnt[4] > 30)
		my->Y -= 10;
	    else if (my->Y-my->Cnt[4] > 20)
		my->Y -= 5;
	    else
		my->Y --;
	}
    }
    else
    {
      if (manage->Loop >1)
      {
        if (my->Cnt[5] % 2 == 0)
        {
          for (i =1; i <manage->Loop; i++)
            ShotToAngle(my->X, my->Y, integerrng()%360, 3);
        }
      }

      my->Cnt[6] += 13;
      if (my->Cnt[6] >= 360)
        my->Cnt[6] -= 360;
      ShotToAngle(my->X,my->Y,my->Cnt[6],4 + manage->Loop * 4);

      my->Cnt[5]++;
    }

    return NoneDel;
}

int NewBoss6(void)
{
    manage->New.Data.hitAtt = MEnemy;
    manage->New.Data.hitMask = MPlayer | MPShot;

    manage->New.Data.X = FieldW/2;
    manage->New.Data.Y = -64;

    manage->New.Data.HP = 400;
    manage->New.Data.Point = 9000;
    manage->New.Data.EnemyAtt = BossDel;

    manage->New.Data.Width = 300;
    manage->New.Data.Height = 100;

    manage->New.Data.Cnt[0] = 0;
    manage->New.Data.Cnt[1] = 200; /* immutable time */
    manage->New.Data.Cnt[2] = 0; /* image counter */

    manage->New.Data.Cnt[3] = 0; /* stealth */
    manage->New.Data.Cnt[4] = 0;  /* shot counter */

    manage->New.Grp.image = Boss6Image;
    return NewObj(MEnemy,BossAct6,BossHit1,DrawImage);
}

DelAtt BossAct6(ObjData *my)
{
    if (my->Cnt[0] < my->Cnt[1])
    {/* appears slowly */
	my->Y++;
	my->Cnt[0]++;
	return NoneDel;
    }

    if (manage->BossTime <= 0)
    {
      if (my->Cnt[3] == 50)
      {
        if ((player->Ships <= 0) && (manage->player[0]->Data.Cnt[3] != 0))
          return NoneDel;
        else
          return BossDel;
      }
    }

    if (manage->BossTime >= 1)
      (manage->BossTime)--;

    if (my->Cnt[3] == 0)
    {
	my->hitAtt = MEnemy;
        my->hitMask = MPlayer | MPShot;
	my->image = 0;
	switch (my->Cnt[4]%10)
	{
	  case 0:
	    if (manage->Loop > 2)
	    {
		RingToPoint(my->X-128,my->Y+32,manage->player[0]->Data.X,manage->player[0]->Data.Y,integerrng()%5+3);
		RingToPoint(my->X-128,my->Y+32,manage->player[0]->Data.X,manage->player[0]->Data.Y,integerrng()%8+3);
		RingToPoint(my->X-128,my->Y+32,manage->player[0]->Data.X,manage->player[0]->Data.Y,integerrng()%3+5);
	    }
	    RingToAngle(my->X-128,my->Y+32,80,8);
	    RingToAngle(my->X-128,my->Y+32,90,8);
	    RingToAngle(my->X-128,my->Y+32,100,8);
	    break;
	  case 2:
	    RingToAngle(my->X,my->Y+32,80,8);
	    RingToAngle(my->X,my->Y+32,90,8);
	    RingToAngle(my->X,my->Y+32,100,8);
	    break;
	  case 4:
	    if (manage->Loop > 2)
	    {
		RingToPoint(my->X+128,my->Y+32,manage->player[0]->Data.X,manage->player[0]->Data.Y,integerrng()%5+3);
		RingToPoint(my->X+128,my->Y+32,manage->player[0]->Data.X,manage->player[0]->Data.Y,integerrng()%8+3);
		RingToPoint(my->X+128,my->Y+32,manage->player[0]->Data.X,manage->player[0]->Data.Y,integerrng()%3+5);
	    }
	    RingToAngle(my->X+128,my->Y+32,80,8);
	    RingToAngle(my->X+128,my->Y+32,90,8);
	    RingToAngle(my->X+128,my->Y+32,100,8);
	    break;
	  case 6:
	    HomingShot(my->X-90,my->Y-20,-15,-10);
	    HomingShot(my->X-60,my->Y-20,0,-10);
	    HomingShot(my->X+60,my->Y-20,0,-10);
	    HomingShot(my->X+90,my->Y-20,15,-10);
	    break;
	  case 8:
            LaserShot(my->X-120 +integerrng()%240,my->Y,20);
	    LaserShot(my->X-120,my->Y,20);
	    LaserShot(my->X+120,my->Y,20);
            if (manage->Loop == 1)
              LaserShot(my->X, my->Y, 20);
            else if (manage->Loop == 2)
            {
              LaserShot(my->X-30,my->Y,20);
              LaserShot(my->X+30,my->Y,20);
            }
            else
            {
              LaserShot(my->X -60, my->Y, 20);
              LaserShot(my->X, my->Y, 20);
              LaserShot(my->X +60,my->Y,20);
            }
	    break;
	}
        if (manage->Loop >2)
          if (my->Cnt[4] %2 == 0)
            if ((manage->player[0]->Data.X < my->X -120)
                ||(manage->player[0]->Data.X > my->X +120)
                ||(manage->player[0]->Data.Y < my->Y))
              ShotToPoint(my->X,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y, 18);
	my->Cnt[4]++;
    }
    else
    {
	my->Cnt[3]--;
	if (my->Cnt[3] < 20)
	{
	    if (my->Cnt[3]%2 == 0)
		my->image = 0;
	    else
		my->image = 1;
	}
    }

    if (my->Cnt[4] == 45)
    {
        ShotToAngle(my->X -120, my->Y, 105, 25);
        ShotToAngle(my->X -120, my->Y, 115, 25);
        ShotToAngle(my->X -120, my->Y, 125, 25);
        ShotToAngle(my->X -120, my->Y, 135, 25);
        ShotToAngle(my->X -120, my->Y, 145, 25);
        ShotToAngle(my->X +120, my->Y, 75, 25);
        ShotToAngle(my->X +120, my->Y, 65, 25);
        ShotToAngle(my->X +120, my->Y, 55, 25);
        ShotToAngle(my->X +120, my->Y, 45, 25);
        ShotToAngle(my->X +120, my->Y, 35, 25);
        if (manage->Loop >1)
        {
          ShotToAngle(my->X -120, my->Y, 155, 25);
          ShotToAngle(my->X -120, my->Y, 165, 25);
          ShotToAngle(my->X -120, my->Y, 175, 25);
          ShotToAngle(my->X +120, my->Y, 25, 25);
          ShotToAngle(my->X +120, my->Y, 15, 25);
          ShotToAngle(my->X +120, my->Y, 5, 25);
        }
    }

    if (my->Cnt[4] >= 50)
    {
	my->Cnt[3] = 50;
	my->Cnt[4] = 0;
        my->hitAtt = 0;
        my->hitMask = 0;
	my->image = 1;
        my->showDamegeTime = 0;

        ShotToAngle(my->X -120, my->Y, 100, 25);
        ShotToAngle(my->X -120, my->Y, 110, 25);
        ShotToAngle(my->X -120, my->Y, 120, 25);
        ShotToAngle(my->X -120, my->Y, 130, 25);
        ShotToAngle(my->X -120, my->Y, 140, 25);
        ShotToAngle(my->X +120, my->Y, 80, 25);
        ShotToAngle(my->X +120, my->Y, 70, 25);
        ShotToAngle(my->X +120, my->Y, 60, 25);
        ShotToAngle(my->X +120, my->Y, 50, 25);
        ShotToAngle(my->X +120, my->Y, 40, 25);
        if (manage->Loop >1)
        {
          ShotToAngle(my->X -120, my->Y, 150, 25);
          ShotToAngle(my->X -120, my->Y, 160, 25);
          ShotToAngle(my->X -120, my->Y, 170, 25);
          ShotToAngle(my->X +120, my->Y, 30, 25);
          ShotToAngle(my->X +120, my->Y, 20, 25);
          ShotToAngle(my->X +120, my->Y, 10, 25);
        }

	my->X = integerrng()%FieldW;
	my->Y = integerrng()%((FieldH -(FieldH%2))/2);
    }

    return NoneDel;
}

int NewBoss7(void)
{
    manage->New.Data.hitAtt = MEnemy;
    manage->New.Data.hitMask = MPlayer | MPShot;

    manage->New.Data.HP = 150;
    manage->New.Data.Point = 8000;
    manage->New.Data.EnemyAtt = BossDel;

    manage->New.Data.Width = 32;
    manage->New.Data.Height = 32;

    manage->New.Data.X = FieldW/2;
    manage->New.Data.Y = 1 - manage->New.Data.Height/2;

    manage->New.Data.Cnt[0] = 0;
    manage->New.Data.Cnt[1] = 0; /* appears slowly */
    manage->New.Data.Cnt[2] = 0; /* image counter */

    do {
	manage->New.Data.Cnt[3] = integerrng()%30 - 15; /* x move */
    }while (abs(manage->New.Data.Cnt[3]) < 5);
    do {
	manage->New.Data.Cnt[4] = integerrng()% 10 + 5; /* y move */
    }while (abs(manage->New.Data.Cnt[4]) < 5);

    manage->New.Data.Cnt[5] = 0;  /* shot counter */

    /* "appeared" counter */
    manage->New.Data.Cnt[6] = 0;

    manage->New.Grp.image = EBoundImage;
    return NewObj(MEnemy,BossAct7,BossHit1,DrawImage);
}

DelAtt BossAct7(ObjData *my)
{
  int i;
    if (manage->BossTime >= 1)
      (manage->BossTime)--;

    my->image++;
    if (my->image >= 8)
        my->image = 0;

    if (manage->BossTime >= 1)
    {
      if (manage->Loop >1)
      {
        if (my->Cnt[5] % 20 == 0)
        {
          for (i = 0; i<360; i+= (90 - (90 % manage->Loop))/ manage->Loop)
            ShotToAngle(my->X, my->Y, i, manage->Loop * 2);
        }
      }

      if (my->Cnt[5] % 30 == 0)
      {
        BoundShot(my->X,my->Y,integerrng()%30-15,integerrng()%30-15, 5);
      }
    }
    

    (my->Cnt[5])++;

    if (my->Cnt[5] >= 60)
      my->Cnt[5] = 0;

    if ((my->Cnt[6] != 0) && (manage->BossTime >= 1))
    {
      if ((my->X+my->Cnt[3]>FieldW) || (my->X+my->Cnt[3]<0))
        my->Cnt[3] = my->Cnt[3]*(-1);
      if ((my->Y+my->Cnt[4]>FieldH) || (my->Y+my->Cnt[4]<0))
        my->Cnt[4] = my->Cnt[4]*(-1);
    }

    if ((my->X < 0 - my->Width/2) || (my->X > FieldW + my->Width/2))
    {
      if ((player->Ships <= 0) && (manage->player[0]->Data.Cnt[3] != 0))
        return NoneDel;
      else
        return BossDel;
    }
    if ((my->Y < 0 - my->Height/2) || (my->Y > FieldH + my->Height/2))
    {
      if ((player->Ships <= 0) && (manage->player[0]->Data.Cnt[3] != 0))
        return NoneDel;
      else
        return BossDel;
    }
      
    my->X += my->Cnt[3];
    my->Y += my->Cnt[4];

    if ((my->Cnt[6] == 0) && (my->X >= 0) && (my->X <= FieldW)
        && (my->Y >= 0) && (my->Y < FieldH))
      my->Cnt[6] = 1;

    return NoneDel;
}

int NewBoss8(void)
{
    manage->New.Data.hitAtt = MEnemy;
    manage->New.Data.hitMask = MPlayer | MPShot;

    manage->New.Data.X = FieldW/2;
    manage->New.Data.Y = -80;

    manage->New.Data.HP = 600;
    manage->New.Data.Point = 10000;
    manage->New.Data.EnemyAtt = BossDel;

    manage->New.Data.Width = 200;
    manage->New.Data.Height = 180;

    manage->New.Data.Cnt[0] = 0;
    manage->New.Data.Cnt[1] = 200; /* appears slowly */
    manage->New.Data.Cnt[2] = 0; /* image counter */
    manage->New.Data.Cnt[3] = 10; /* x move */
    manage->New.Data.Cnt[4] = 0; /* y move */
    manage->New.Data.Cnt[5] = 0;  /* shot counter */
    manage->New.Data.Cnt[6] = integerrng()%4; /* mode */
    manage->New.Data.Cnt[7] = 0; /* mode counter */
    manage->New.Data.Cnt[8] = 0; /* after-death counter */

    manage->New.Grp.image = Boss7Image;
    return NewObj(MEnemy,BossAct8,BossHit8,DrawImage);
}

DelAtt BossAct8(ObjData *my)
{
    if (my->Cnt[0] < my->Cnt[1])
    {/* appears slowly */
	my->Y++;
	my->Cnt[0]++;
	return NoneDel;
    }

    if (my->kill == True)
    {
      /* the boss is dead */
      return BossAct8_dead(my);
    }

    /* the boss is alive */
    if (manage->BossTime <= 0)
    {
      if (my->Y < 0 - my->Height/2 - 50)
      {
        if ((player->Ships > 0) ||(manage->player[0]->Data.Cnt[3] == 0))
          return BossDel;
        else
          return NoneDel;
      }
      else
      {
        (my->Y) -= 4;
        return NoneDel;
      }
    }

    (manage->BossTime)--;

    if (my->Cnt[5] > 8)
	my->Cnt[5] = 0;
    else
	my->Cnt[5]++;

    if ((my->Cnt[5]%2==0) && ((my->Cnt[6] ==0) || (my->Cnt[6] == 2)
                              || (my->Cnt[6] == 3)))
    {
	RingToAngle(my->X, my->Y-60, integerrng()%50+70, integerrng()%15+5);
	RingToAngle(my->X, my->Y-60, integerrng()%20+80, integerrng()%10+10);
        /*
	if (manage->Loop > 2)
	    RingToAngle(my->X, my->Y-60, integerrng()%20+80, integerrng()%5+15);
        */
    }

    if (my->Cnt[6] != 4)
    {
	if (my->X>manage->player[0]->Data.X && my->X>my->HarfW)
	    my->X--;
	else if (my->X<manage->player[0]->Data.X && my->X<(FieldW-my->HarfW))
	    my->X++;
    }

    if (my->Cnt[6] == 0)
    {
      return BossAct8_rand_shot(my);
    }
    else if (my->Cnt[6] == 1)
    {
      return BossAct8_ring_shot(my);
    }
    else if (my->Cnt[6] == 2)
    {
      return BossAct8_bound_shot(my);
    }
    else if (my->Cnt[6] == 3)
    {
      return BossAct8_homing_shot(my);
    }
    else if (my->Cnt[6] == 4)
    {
      return BossAct8_charge_shot(my);
    }
    else if (my->Cnt[6] == 5)
    {
      return BossAct8_n_way_shot(my);
    }

    return NoneDel;
}

DelAtt BossHit8(ObjData *my, ObjData *your)
{
  int temp = your->Attack;
    if (my->Cnt[0] < my->Cnt[1])
	return NoneDel;
    /*
    if (my->Cnt[6] == 4)
	return NoneDel;
    */
    /* immutable in above cases */
    if (my->HP < your->Attack)
      temp = my->HP;

    my->HP -= temp;
    player->Rec[0].score += temp;

    if (my->HP <= 0)
    {
       player->Rec[0].score -= 1;     
       my->showDamegeTime = 0;
        
       my->kill = True;
    }
    else
    {
      my->showDamegeTime = 15;
    }
    
    return NoneDel;
}

static DelAtt BossAct8_dead(ObjData *my)
{
  int i;
  int j;
  /* the boss is dead */
  NewLargeBomb(my->X+integerrng()%230-115,my->Y+integerrng()%180-90);
  NewLargeBomb(my->X+integerrng()%230-115,my->Y+integerrng()%180-90);

  my->Cnt[8]++;
  if ((player->Ships <= 0) && (manage->player[0]->Data.Cnt[3] != 0))
    if (my->Cnt[8] >= 300)
      my->Cnt[8] = 0;
  
  if (my->Cnt[8] %3 == 0)
  {
    if (my->X > manage->player[0]->Data.X)
      (my->X)--;
    else if (my->X < manage->player[0]->Data.X)
      (my->X)++;
    if (my->Y > manage->player[0]->Data.Y)
      (my->Y)--;
    else if (my->Y < manage->player[0]->Data.Y)
      (my->Y)++;
  }
  
  if (my->Cnt[8] %30 == 0)
  {
    for (i=0; i<24; i++)
      ShotToAngle(my->X,my->Y,i*15, 7);
  }
  else if (my->Cnt[8] %30 == 15)
  {
    for (i=0; i<24; i++)
      ShotToAngle(my->X,my->Y,i*15 + 7,7);
  }
  if (my->Cnt[8] %13 == 0)
  {
    ShotToPoint(my->X,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y, manage->Loop *5);
    if (manage->Loop >2)
    {
      BoundShot(my->X, my->Y-60,integerrng()%20-10,integerrng()%4+10,4);
    }
  }
  if (my->Cnt[8] %5 == 0)
  {
    if (manage->Loop >1)
    {
      for (i=1; i< manage->Loop * 3; i++)
        ShotToAngle(my->X,my->Y,integerrng()%360, manage->Loop *3);
    }
  }
  
  if (my->Cnt[8] >= 300)
  {
    for (i=0; i<=10; i++)
      for (j=0; j<=10; j++)
        NewLargeBomb(50*i,65*j);
    return my->EnemyAtt;
  }
  else
    return NoneDel;
}

static DelAtt BossAct8_rand_shot(ObjData *my)
{
  int i;
  if (my->Cnt[7]% 10 == 0)
  {
    for (i = 1; i <= manage->Loop; i++)
    {
      ShotToAngle(my->X-60, my->Y-60, integerrng()%50 + 70, integerrng()%5+5);
      ShotToAngle(my->X+60, my->Y-60, integerrng()%50 + 70, integerrng()%5+5);
    }
    ShotToPoint(my->X,my->Y-60,manage->player[0]->Data.X,manage->player[0]->Data.Y,integerrng()%5+3);
  }

  BossAct8_next(my, 100);
  return NoneDel;
}

static DelAtt BossAct8_ring_shot(ObjData *my)
{
  if (my->Cnt[7]%2 == 0)
  {
    RingToPoint(my->X-60, my->Y+10, manage->player[0]->Data.X,manage->player[0]->Data.Y, integerrng()%10 + 20);
    RingToPoint(my->X+60, my->Y+10, manage->player[0]->Data.X,manage->player[0]->Data.Y, integerrng()%10 + 20);
    if (manage->Loop >1)
    {
      RingToAngle(my->X, my->Y - 60, 90, integerrng()%10 + 20);
      RingToAngle(my->X, my->Y - 60, 90, integerrng()%10 + 20);
    }
    if (manage->Loop >2)
    {
      RingToAngle(my->X-60, my->Y - 60, 90, integerrng()%10 + 20);
      RingToAngle(my->X+60, my->Y - 60, 90, integerrng()%10 + 20);
    }
  }

  BossAct8_next(my, 70);
  return NoneDel;
}

static DelAtt BossAct8_bound_shot(ObjData *my)
{
  if (manage->Loop <=2)
  {
    if (my->Cnt[7]  % 16 == 0)
      BoundShot(my->X+60,my->Y-60,integerrng()%20-10,integerrng()%4+10,2);
    else if (my->Cnt[7]  % 16 == 8)
      BoundShot(my->X-60,my->Y-60,integerrng()%20-10,integerrng()%4+10,2);
  }
  else
  {
    if (my->Cnt[7]  % 16 == 0)
      BoundShot(my->X+60,my->Y-60, (integerrng()%20-10)*2, (integerrng()%4+10)*2, 2);
    else if (my->Cnt[7]  % 16 == 8)
      BoundShot(my->X-60,my->Y-60, (integerrng()%20-10)*2 ,(integerrng()%4+10)*2, 2);
  }

  BossAct8_next(my, 100);
  return NoneDel;
}

static DelAtt BossAct8_homing_shot(ObjData *my)
{
  if (my->Cnt[7] %8 == 0)
  {
    HomingShot(my->X-60,my->Y-10,-10, 5);
    HomingShot(my->X+60,my->Y-10, 10, 5);
    if (manage->Loop > 2)
    {
      HomingShot(my->X-60,my->Y-10,-3, 10);
      HomingShot(my->X+60,my->Y-10, 3, 10);
    }
  }

  BossAct8_next(my, 100);
  return NoneDel;
}

static DelAtt BossAct8_charge_shot(ObjData *my)
{
  int i;
  if (my->Cnt[7] == 48 - manage->Loop *8)
  {
    for (i=0; i<24; i++)
      ShotToAngle(my->X,my->Y,i*15,15);
  }
  if (my->Cnt[7] == 53 - manage->Loop *8)
  {
    for (i=0; i<24; i++)
      ShotToAngle(my->X,my->Y,i*15+7,20);
  }
  if (my->Cnt[7] > 53 - manage->Loop *8)
  {
    LaserShot(my->X-120,my->Y-60,35);
    LaserShot(my->X-80 ,my->Y-60,35);
    LaserShot(my->X-40 ,my->Y-60,35);
    LaserShot(my->X    ,my->Y-60,35);
    LaserShot(my->X+40 ,my->Y-60,35);
    LaserShot(my->X+80 ,my->Y-60,35);
    LaserShot(my->X+120,my->Y-60,35);
  }

  BossAct8_next(my, 58 - manage->Loop *8);
  return NoneDel;
}

static DelAtt BossAct8_n_way_shot(ObjData *my)
{
  int xtemp;
  int ytemp;
  int speedtemp;
  if (my->Cnt[7] % 13 == 0)
  {
    xtemp = integerrng()%FieldW;
    ytemp = integerrng()%((FieldH -(FieldH%2))/2);
    speedtemp = integerrng()%7 + 5;

    if (manage->Loop == 1)
    {
      ShotToAngle(xtemp, ytemp, 50, speedtemp);
      ShotToAngle(xtemp, ytemp, 70, speedtemp);
      ShotToAngle(xtemp, ytemp, 90, speedtemp);
      ShotToAngle(xtemp, ytemp, 110, speedtemp);
      ShotToAngle(xtemp, ytemp, 130, speedtemp);
    }
    else if (manage->Loop == 2)
    {
      ShotToAngle(xtemp, ytemp, 51, speedtemp);
      ShotToAngle(xtemp, ytemp, 64, speedtemp);
      ShotToAngle(xtemp, ytemp, 77, speedtemp);
      ShotToAngle(xtemp, ytemp, 90, speedtemp);
      ShotToAngle(xtemp, ytemp, 103, speedtemp);
      ShotToAngle(xtemp, ytemp, 116, speedtemp);
      ShotToAngle(xtemp, ytemp, 129, speedtemp);
    }
    else
    {
      ShotToAngle(xtemp, ytemp, 50, speedtemp);
      ShotToAngle(xtemp, ytemp, 60, speedtemp);
      ShotToAngle(xtemp, ytemp, 70, speedtemp);
      ShotToAngle(xtemp, ytemp, 80, speedtemp);
      ShotToAngle(xtemp, ytemp, 90, speedtemp);
      ShotToAngle(xtemp, ytemp, 100, speedtemp);
      ShotToAngle(xtemp, ytemp, 110, speedtemp);
      ShotToAngle(xtemp, ytemp, 120, speedtemp);
      ShotToAngle(xtemp, ytemp, 130, speedtemp);
    }
  }

  BossAct8_next(my, 150);
  return NoneDel;
}

static void BossAct8_next(ObjData *my, int span)
{
  int modetemp = 0;
  if (my->Cnt[7] > span)
  {
    do
    {
      modetemp = integerrng()%6;
    }while (modetemp == my->Cnt[6]);
    my->Cnt[6] = modetemp;
    my->Cnt[7] = 0;
  }
  else
    my->Cnt[7]++;
}







#include "enemy.h"
#include "extern.h"
#include "key.h"
/* ClearEnemyShotManage, ClearManage, DelObj */
#include "manage.h"
#include "graphic.h"
#include "input.h"


/* DamageHit, LargeDamageHit
 * DrawRec if DEBUG
 */
#include "callback.h"
#include "game.h"

/* local functions */
static void DrawInfo(void);

static void do_actions(void);
static void collision_detection(void);

static int shoot_down_bonus(int percent, int loop, int stage);
/*
static int perfect_bonus(int loop, int stage);
*/

#include "data.h"

int mainLoop( void ) {
    int obj; /* loop counter */

    int ocheck; /* counter for already checked objects */


    int oneUp = 0; /* 1up counter */

    /* functions of normal enemies, weak ones first */
    int (*NewEnemy[])(int x, int y) = 
    {
	NewEnemy1,NewEnemy2,NewEnemy3,NewEnemy4,NewEnemy5,
	NewEnemy6,NewEnemy7,NewEnemy8,NewEnemy9
    };

    /* functions of end-of-stage bosses */
    int (*NewBoss[])(void) =
    {
	NewBoss1,NewBoss2,NewBoss3,NewBoss4,NewBoss5,
	NewBoss6,NewBoss7,NewBoss8
    };

    /* number of normal enemies in each stage */    
    int StageObj[] = 
    {
	80,80,100,100,120,120,140,140
    };

    char StageName[][16] = 
    {
	"Stage 1","Stage 2","Stage 3","Stage 4","Stage 5",
	"Stage 6","Stage 7","Final Stage",
	"All Clear!"
    };
    
    /* add the player ship to the table */
    NewPlayer(FieldW/2,FieldH - 32);
    XFlush(dpy);
    for ( int record_data_index = 0; record_data_index<RECORD_DATA_SIZE; record_data_index++ ) {
        keymask = record_data[record_data_index];

	if ( manage->Stage>MaxStage && manage->Appear>0) { break; }
	if ( manage->player[0]->Data.used == False ){
          /* the player is killed */
	    player->Rec[0].loop = manage->Loop;
	    player->Rec[0].stage = manage->Stage;
	    if (player->Ships > 0)
	    {
		player->Ships--;
                ClearEnemyShotManage(manage);
                PlayerLosePower();
		RestartPlayer(FieldW/2,FieldH - 32);
	    }
	}
	
	if (manage->BossKill == True)
	{
          /* the boss is killed, or it escaped */
          manage->Stage++;

          manage->Level += 5;

          if (manage->BossTime >=1)
          {
            /* the boss is dead */
	    player->Percent = (manage->StageShotDown/(double)manage->StageEnemy) * 100;
            /* note that manage->Stage is already incremented */
	    player->Rec[0].score += shoot_down_bonus(player->Percent, manage->Loop, manage->Stage);
	    if (player->Percent >= 100)
            {
              /*
              player->Rec[0].score += perfect_bonus(manage->Loop, manage->Stage);
              */
              manage->Level += 7;
            }
            
          }
          else
          {
            manage->Level -= 3;
          }
          if (manage->Level > MaxLevel)
            manage->Level = MaxLevel;
          if (manage->Level < 0)
            manage->Level = 0;

          if (manage->flag_maxlevel == True){
            manage->Level = MaxLevel;
          }

	    ClearManage(manage);
	}

	if (manage->Appear >= 100)
	{
	    if ((manage->StageEnemy >= StageObj[manage->Stage-1]) && (manage->BossApp==False))
	    {
              /* the boss appears */
		if (NewBoss[manage->Stage-1]() != -1)
		{
                  manage->ZakoApp = False;
		    manage->BossApp = True;
		    manage->StageEnemy++;
                    if (manage->Stage == 8)
                      manage->BossTime = 3000;
                    else
                      manage->BossTime = 2000;
		}
	    }
	    else if (manage->ZakoApp == True)
	    {
              /* normal enemy */
		if (NewEnemy[integerrng()%(manage->Stage+1)]((integerrng()%FieldW)+1,0) != -1)
		{
		    manage->StageEnemy++;
		}
	    }
	    /* how often normal enemies appear */
            manage->Appear = 89;
	}
	else
	    manage->Appear++;

        do_actions();
	
        collision_detection();
        

	/* the player gets 1up? */
	if (player->Rec[0].score >= player->Next)
	{
	    player->Next += EVERY1UP;
	    player->Ships++;

            /* counter to display 1up message */
	    oneUp = 1;
	}

        /* draw the window */
        clear_window();

	/* pixmaps for objects */
	for (obj=0,ocheck=0; (obj<manage->EnemyMax && ocheck<manage->EnemyNum); obj++)
	{
	    if (manage->enemy[obj]->Data.used == True)
	    {
		manage->enemy[obj]->Realize(&(manage->enemy[obj]->Data),&(manage->enemy[obj]->Grp));
		ocheck++;
	    }
	}
	for (obj=manage->PlayerMax-1,ocheck=0; (obj>=0 && ocheck<manage->PlayerNum); obj--)
	{
	    if (manage->player[obj]->Data.used == True)
	    {
		manage->player[obj]->Realize(&(manage->player[obj]->Data),&(manage->player[obj]->Grp));
		ocheck++;
	    }
	}
        if (player->Rec[0].score >= 10000000)
          player->Rec[0].score = 10000000;
        
	/* score and other stuff */
	DrawInfo();

	/* yet more misc stuff */
	if (oneUp != 0)
	{
	    if (oneUp%4 > 1)
              draw_string(440, 620, "1UP", strlen("1UP"));
	    oneUp++;
	    if (oneUp > 50)
		oneUp = 0;
	}

	if (manage->player[0]->Data.kill==True && player->Ships==0)
          draw_string(230, 300, "Game Over", strlen("Game Over"));

	if (manage->Appear < 0)
	{
	    char Percent[32];
	    char Bonus[32];
	    char Perfect[32];

	    if (manage->showShootDown != 0)
	    {
              /* shoot down bonus message */
              if (manage->BossTime >= 1)
              {
		sprintf(Percent,"shoot down %02d%%",player->Percent);
                draw_string(210, 370, Percent, strlen(Percent));


		sprintf(Bonus,"Bonus %d pts", shoot_down_bonus(player->Percent, manage->Loop, manage->Stage));
                draw_string(260 + manage->Appear*3 , 400,
                            Bonus, strlen(Bonus));

		if (player->Percent >= 100)
		{
		    sprintf(Perfect,"Perfect!!");
                    draw_string(170 - manage->Appear*3 , 420,
                                Perfect, strlen(Perfect));
		}
              }
              else
              {
                snprintf(Percent, 32, "the boss escaped");
                draw_string(200 ,370 ,Percent, strlen(Percent));
              }
              
	    }
            draw_string(230, 320, StageName[manage->Stage-1],
                        strlen(StageName[manage->Stage-1]));
	}

        redraw_window();
    }

    /* ending */
    return 1;
}

/* show score and other info */
static void DrawInfo(void)
{
    static char Score[64];
    static char Ships[16];
    static char Stage[16];
#ifdef DEBUG
    static char ObjectP[32];
    static char ObjectE[32];
    static char Loop[16];
    static char Level[16];
    static char Weapon[16];
    static char Pow[16];
    static char Speed[16];
    static char Enemy[16];
    static char EnemyKill[16];
#endif
    static char EnemyHP[5];
    static char BossTime[16];
    
    int i;

    sprintf(Score,"Score % 8d",player->Rec[0].score);
    sprintf(Stage,"Stage %2d",manage->Stage);
    sprintf(Ships,"Ships %3d",player->Ships);
#ifdef DEBUG
    sprintf(ObjectE,"Enemy Object %3d",manage->EnemyNum);
    sprintf(ObjectP,"Player Object %3d",manage->PlayerNum);
    sprintf(Loop,"Loop %2d",manage->Loop);
    sprintf(Level,"Level %3d",manage->Level);
    sprintf(Weapon,"Weapon %d",manage->player[0]->Data.Cnt[5]);
    sprintf(Pow,"Pow %2d",manage->player[0]->Data.Cnt[6]);
    sprintf(Speed,"Speed %2d",manage->player[0]->Data.Speed);
    sprintf(Enemy,"Enemy %3d",manage->StageEnemy);
    sprintf(EnemyKill,"EnemyKill %3d",manage->StageShotDown);
#endif

    draw_string(10, 20, Score, strlen(Score));
    draw_string(430, 20, Stage, strlen(Stage));
    draw_string(430, 640, Ships, strlen(Ships));
#ifdef DEBUG
    draw_string(10, 40, ObjectE, strlen(ObjectE));
    draw_string(10, 60, ObjectP, strlen(ObjectP));
    draw_string(10, 80, Level, strlen(Level));
    draw_string(10, 100, Enemy, strlen(Enemy));
    draw_string(10, 120, EnemyKill, strlen(EnemyKill));
    draw_string(430, 60, Loop, strlen(Loop));
    draw_string(430, 580, Weapon, strlen(Weapon));
    draw_string(430, 600, Pow, strlen(Pow));
    draw_string(430, 620, Speed, strlen(Speed));
#endif
    for (i = 0; i<manage->EnemyMax; i++)
      if (manage->enemy[i]->Data.used == True)
        if ((manage->enemy[i]->Hit == EnemyHit1)
            ||(manage->enemy[i]->Hit == DamageHit)
            ||(manage->enemy[i]->Hit == LargeDamageHit)
            ||(manage->enemy[i]->Hit == BossHit1)
            ||(manage->enemy[i]->Hit == BossHit8))
          if (manage->enemy[i]->Data.showDamegeTime >0)
          {
            snprintf(EnemyHP, 5, "%d",manage->enemy[i]->Data.HP);
            draw_string(manage->enemy[i]->Data.X, manage->enemy[i]->Data.Y,
                        EnemyHP, strlen(EnemyHP));
            (manage->enemy[i]->Data.showDamegeTime)--;
          }
    if (manage->BossApp == True)
    {
      snprintf(BossTime, 16, "Time %4d",manage->BossTime);
      draw_string(430, 40, BossTime, strlen(BossTime));
    }

#ifdef DEBUG
    for (i = 0; i<manage->EnemyMax; i++)
      if (manage->enemy[i]->Data.used == True)
        /* DrawRec does not use arg 2, so NULL will be enough */
        DrawRec(&(manage->enemy[i]->Data), NULL);
    for (i = 0; i<manage->PlayerMax; i++)
      if (manage->player[i]->Data.used == True)
        /* DrawRec does not use arg 2, so NULL will be enough */
        DrawRec(&(manage->player[i]->Data), NULL);
        
#endif /* DEBUG */



    
}

static void do_actions(void)
{
  int obj;
  DelAtt DelFlag;

  for (obj=manage->PlayerMax-1; obj>=0; obj--)
    manage->player[obj]->Data.shouldAct = manage->player[obj]->Data.used;
  for (obj=manage->PlayerMax-1; obj>=0; obj--)
  {
    if (manage->player[obj]->Data.shouldAct == True)
    {
      if (manage->player[obj]->Action(&(manage->player[obj]->Data)) == NullDel)
        DelObj(manage->player[obj]);
    }
  }
  
  for (obj=0; obj<manage->EnemyMax; obj++)
    manage->enemy[obj]->Data.shouldAct = manage->enemy[obj]->Data.used;
  for (obj=0; obj<manage->EnemyMax; obj++)
  {
    if (manage->enemy[obj]->Data.used == True)
    {
      DelFlag = manage->enemy[obj]->Action(&(manage->enemy[obj]->Data));
      switch (DelFlag)
      {
      case NoneDel:
        /* do nothing */
        break;
      case BossDel:
#ifdef DEBUG
        fprintf(stderr, "DelFlag == BossDel while processing Action: obj = %d\n", obj);
#endif
        if ((DelFlag == BossDel) && (manage->BossTime <= 0))
          player->Rec[0].score -= manage->enemy[obj]->Data.Point;
        manage->BossKill = True;
        /* fall off */
      case ZakoDel:
        player->Rec[0].score += manage->enemy[obj]->Data.Point;
        manage->StageShotDown++;
        /* fall off */
      case NullDel:
        DelObj(manage->enemy[obj]);
        break;
      }
    }
  }
}


static void collision_detection(void)
{
  int obj;
  int target;
  DelAtt DelFlag;
  
  for (obj=0; (obj<manage->PlayerMax); obj++)
  {
    if (manage->player[obj]->Data.used == False)
      continue;
    if (manage->player[obj]->Data.kill == True)
      continue;
    
    for (target=0; (target<manage->EnemyMax); target++)
    {
      if (manage->enemy[target]->Data.used == False)
        continue;
      if (manage->enemy[target]->Data.kill == True)
        continue;
      
      if (manage->player[obj]->Data.hitMask & manage->enemy[target]->Data.hitAtt)
      {
        if (abs(manage->player[obj]->Data.X-manage->enemy[target]->Data.X)
            > manage->player[obj]->Data.HarfW+manage->enemy[target]->Data.HarfW)
          continue;
        if (abs(manage->player[obj]->Data.Y-manage->enemy[target]->Data.Y)
            > manage->player[obj]->Data.HarfH+manage->enemy[target]->Data.HarfH)
          continue;
        
        /* crash! */
        /* we call the enemy's Hit first because the Hit of
         * the player shot 3 changes its Attatck */
        DelFlag = manage->enemy[target]->Hit(&(manage->enemy[target]->Data),&(manage->player[obj]->Data));
        switch (DelFlag)
        {
        case NoneDel:
          /* ignore it */
          break;
          
        case BossDel:
#ifdef DEBUG
          fprintf(stderr, "DelFlag == BossDel while processing Hit: target = %d, obj = %d\n", target, obj);
#endif
          if ((DelFlag == BossDel) && (manage->BossTime <= 0))
            player->Rec[0].score -= manage->enemy[target]->Data.Point;
          manage->BossKill = True;
        case ZakoDel:
          player->Rec[0].score += manage->enemy[target]->Data.Point;
          manage->StageShotDown++;
        case NullDel:
          DelObj(manage->enemy[target]);
        }
        
        if (manage->player[obj]->Hit(&(manage->player[obj]->Data),&(manage->enemy[target]->Data)) == NullDel)
          DelObj(manage->player[obj]);
      }
    }
  }
}

static int shoot_down_bonus(int percent, int loop, int stage)
{
  if (percent < 0)
  {
    fprintf(stderr, "shoot_down_bonus: negative percent given, "
            "assuming 0%%\n");
    percent = 0;
  }
  if (percent > 100)
  {
    fprintf(stderr, "shoot_down_bonus: 101+ percent given, "
            "assuming 100%%\n");
    percent = 100;
  }
  if (percent == 0)
    return 0;
  
  
  return (30000 + stage * stage * 1000) * 5 / (105 - percent);
}

/* xsoldier, a shoot 'em up game with "not shooting" bonus
 * Copyright (C) 1997 Yuusuke HASHIMOTO <s945750@educ.info.kanagawa-u.ac.jp>
 * Copyright (C) 2002 Oohara Yuuma  <oohara@libra.interq.or.jp>
 *
 * This is a copyleft program.  See the file LICENSE for details.
 */
/* $Id: player.c,v 1.12 2011/08/12 14:33:57 oohara Exp $ */

/* DEBUG is defined in config.h */
#include <config.h>
/* rand */
#include <stdlib.h>

/*
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/xpm.h>
*/

#include "image.h"
#include "xsoldier.h"
#include "manage.h"
#include "player.h"
#include "common.h"
#include "callback.h"
#include "extern.h"
#include "key.h"
/* NewEnemy10 (power-up item) */
#include "enemy.h"

/* player object */
void NewPlayer(int x, int y)
{
    manage->player[0]->Data.hitAtt = MPlayer;
    manage->player[0]->Data.hitMask = MEnemy | MEShot | MItem;

    manage->player[0]->Data.used = True;
    manage->player[0]->Data.kill = False;
    manage->player[0]->Data.X = x;
    manage->player[0]->Data.Y = y;
    manage->player[0]->Data.Speed = 15;
    manage->player[0]->Data.Attack = 0;
    manage->player[0]->Data.Width = 16;
    manage->player[0]->Data.Height = 16;
    manage->player[0]->Data.HarfW = manage->player[0]->Data.Width/2;
    manage->player[0]->Data.HarfH = manage->player[0]->Data.Height/2;
    manage->player[0]->Data.image = 0;

    manage->player[0]->Data.Cnt[0] = 0; /* auto shot counter */
    manage->player[0]->Data.Cnt[1] = 0; /* image counter */
    manage->player[0]->Data.Cnt[2] = 60; /* immutable counter */
    manage->player[0]->Data.Cnt[3] = 0; /* death coounter */
    manage->player[0]->Data.Cnt[4] = False; /* transparent flag */
    manage->player[0]->Data.Cnt[5] = 1; /* weapon counter */
    manage->player[0]->Data.Cnt[6] = manage->start_power; /* power counter */
    
    manage->player[0]->Grp.image = PlayerImage;
    /*
    manage->player[0]->Grp.Width = manage->player[0]->Grp.image[0]->width;
    manage->player[0]->Grp.Height = manage->player[0]->Grp.image[0]->height;
    */
    manage->player[0]->Grp.Width = 32;
    manage->player[0]->Grp.Height = 32;
    manage->player[0]->Grp.HarfW = manage->player[0]->Grp.Width/2;
    manage->player[0]->Grp.HarfH = manage->player[0]->Grp.Height/2;
    
    manage->player[0]->Action = PlayerAction;
    manage->player[0]->Hit = PlayerHit;
    manage->player[0]->Realize = DrawImage;
    manage->player[0]->Data.notShootingTime = 0;

    manage->PlayerNum++;
}

void RestartPlayer(int x, int y)
{
    manage->player[0]->Data.hitAtt = MPlayer;
    manage->player[0]->Data.hitMask = MEnemy | MEShot | MItem;
    manage->player[0]->Data.used = True;
    manage->player[0]->Data.kill = False;
    manage->player[0]->Data.X = x;
    manage->player[0]->Data.Y = y;

    manage->player[0]->Data.Cnt[0] = 0; /* auto shot counter */
    manage->player[0]->Data.Cnt[1] = 0; /* image counter */
    manage->player[0]->Data.Cnt[2] = 60; /* immutable counter */
    manage->player[0]->Data.Cnt[3] = 0; /* death counter */
    manage->player[0]->Data.Cnt[4] = False; /* transparent flag */
    manage->player[0]->Data.notShootingTime = 0;
    
    manage->PlayerNum++;
}

DelAtt PlayerAction(ObjData *my)
{
    /* you can change speed at any time */
    if (keymask & SpeedUP)
    {
	if (my->Speed < 30)
	    my->Speed += 3;
	keymask -= SpeedUP;
    }
    if (keymask & SpeedDOWN)
    {
	if (my->Speed > 6)
	    my->Speed -= 3;
	keymask -= SpeedDOWN;
    }

    if (joymask & SpeedUP)
    {
	if (my->Speed < 30)
	    my->Speed += 3;
    }
    if (joymask & SpeedDOWN)
    {
	if (my->Speed > 6)
	    my->Speed -= 3;
    }

    /* your action is over if you are dead */
    if (my->kill == True)
    {
	my->Cnt[3]++;
	if (my->Cnt[3] == 30)
	    return NullDel;

	NewLargeBomb(my->X+integerrng()%20-10,my->Y+integerrng()%20-10);
	return NoneDel;
    }

    /* move */
    if (keymask & Up || joymask & Up)
	my->Y -= my->Speed;
    if (keymask & Down || joymask & Down)
	my->Y += my->Speed;
    if (keymask & Left || joymask & Left)
    {
	my->X -= my->Speed;
	my->Cnt[1] -= 2;
    }
    if (keymask & Right || joymask & Right)
    {
	my->X += my->Speed;
	my->Cnt[1] += 2;
    }

    /* choose image */
    if (my->Cnt[1] > 0)
	my->Cnt[1]--;
    else if (my->Cnt[1] < 0)
	my->Cnt[1]++;

    if (my->Cnt[1] > 10)
	my->image = 4;
    else if (my->Cnt[1] > 0)
	my->image = 3;
    else if (my->Cnt[1] < -10)
	my->image = 2;
    else if (my->Cnt[1] < 0)
	my->image = 1;
    else
	my->image = 0;

    if (my->Cnt[2] != 0)
    {
	if (my->Cnt[4] == False)
	{
	    my->image = 5;
	    my->Cnt[4] = True;
	}
	else
	    my->Cnt[4] = False;
    }

    /* boundary check for moving */
    if (my->X - 16 < 0)
	my->X = 0 + 16;
    else if (my->X + 16 > FieldW)
	my->X = FieldW - 16;
    if (my->Y - 16 < 0)
	my->Y = 0 + 16;
    else if (my->Y + 16 > FieldH)
	my->Y = FieldH - 16;

    if (my->Cnt[2] > 0)
	my->Cnt[2]--;

    if (keymask & Shot || joymask & Shot)
    {
      my->notShootingTime = 5;
	if (my->Cnt[0] == 0)
	{
	    switch (my->Cnt[5])
	    {
	      case 1:
                /* how often you can shoot */
		if (my->Cnt[6] >= 30)
		    my->Cnt[0] = 2;
		else
		    my->Cnt[0] = 3;

		if (my->Cnt[6] >= 20)
		{
                  PlayerShot1(my->X-10,my->Y,35,90, my->Cnt[6]);
                  PlayerShot1(my->X+10,my->Y,35,90, my->Cnt[6]);
		}
                else if (my->Cnt[6] >= 10)
                  PlayerShot1(my->X, my->Y,35,90, my->Cnt[6]);

		if (my->Cnt[6] >= 15)
		{
                  PlayerShot1(my->X-40,my->Y+10,35,270, my->Cnt[6]);
                  PlayerShot1(my->X-10,my->Y,35,270, my->Cnt[6]);
                  PlayerShot1(my->X+10,my->Y,35,270, my->Cnt[6]);
                  PlayerShot1(my->X+40,my->Y+10,35,270, my->Cnt[6]);
		}
                else if (my->Cnt[6] >= 5)
                {
                  PlayerShot1(my->X-25,my->Y+10,35,270, my->Cnt[6]);
                  PlayerShot1(my->X, my->Y,35,270, my->Cnt[6]);
                  PlayerShot1(my->X+25, my->Y+10,35,270, my->Cnt[6]);
                }
                else
                {
                  PlayerShot1(my->X-10,my->Y,35,270, my->Cnt[6]);
                  PlayerShot1(my->X+10,my->Y,35,270, my->Cnt[6]);
                }
                
		break;
	      case 2:
                /* how often you can shoot */
		if (my->Cnt[6] >= 25)
		    my->Cnt[0] = 1;
		else if (my->Cnt[6] >= 19)
		    my->Cnt[0] = 2;
		else
		    my->Cnt[0] = 3;
                PlayerShot2(my->X,my->Y,25,255);
		PlayerShot2(my->X,my->Y,25,270);
		PlayerShot2(my->X,my->Y,25,285);
                if (my->Cnt[6] >= 15)
		{
		    PlayerShot2(my->X,my->Y,25,0);
		    PlayerShot2(my->X,my->Y,25,180);
		}
                if (my->Cnt[6] >= 11)
		{
		    PlayerShot2(my->X,my->Y,25,225);
		    PlayerShot2(my->X,my->Y,25,315);
		}
		if (my->Cnt[6] >= 7)
		{
		    PlayerShot2(my->X,my->Y,25,60);
		    PlayerShot2(my->X,my->Y,25,120);
		}
		if (my->Cnt[6] >= 3)
		{
		    PlayerShot2(my->X,my->Y,25,240);
		    PlayerShot2(my->X,my->Y,25,300);
		}

		break;
	      case 3:
                /* how often you can shoot */
		if (my->Cnt[6] >= 30)
		    my->Cnt[0] = 3;
		else
		    my->Cnt[0] = 4;
		if (my->Cnt[6] >= 10)
		{
		    PlayerShot3(my->X,my->Y-10,0, my->Cnt[6]);
		    PlayerShot3(my->X-10,my->Y-10,-1, my->Cnt[6]);
		    PlayerShot3(my->X+10,my->Y-10,1, my->Cnt[6]);
		}
                else if (my->Cnt[6] >= 5)
		{
		    PlayerShot3(my->X-10,my->Y-10,0, my->Cnt[6]);
		    PlayerShot3(my->X+10,my->Y-10,0, my->Cnt[6]);
		}
		else
		    PlayerShot3(my->X,my->Y-10,0, my->Cnt[6]);
		break;
	    }
	}
	else
	    my->Cnt[0]--;
    }
    else
    {
      if (my->notShootingTime <= 0)
      {
        /* releasing the shoot button gives you a score */
        player->Rec[0].score += (manage->EnemyNum) * 3;
      }
      else
        (my->notShootingTime)--;
      
      my->Cnt[0] = 0;
    }
    
    return NoneDel;
}

DelAtt PlayerHit(ObjData *my, ObjData *your)
{
    if (your->hitAtt == MItem)
    {
	if (your->Cnt[0] == 0)
	    my->Cnt[6] += 2;
	else
        {
          my->Cnt[6]++;
          my->Cnt[5] = your->Cnt[0];
        }
        
	for(; my->Cnt[6] > 45; (my->Cnt[6])--)
        {
          player->Rec[0].score += 1000 * manage->Loop;
          (manage->Level)++;
        }
          
        if (manage->Level > MaxLevel)
          manage->Level = MaxLevel;
        if (manage->Level < 0)
          manage->Level = 0;

        if (manage->flag_maxlevel == True)
          manage->Level = MaxLevel;

	return NoneDel;
    }

    if (my->Cnt[2] == 0)
    {
	my->kill = True;
	my->image = 5;
        manage->Level -= 5;
        if (manage->Level > MaxLevel)
          manage->Level = MaxLevel;
        if (manage->Level < 0)
          manage->Level = 0;

        if (manage->flag_maxlevel == True)
          manage->Level = MaxLevel;
    }
    return NoneDel;
}

/* player shot */
void PlayerShot1(int x, int y, int speed, int angle, int attack)
{
    manage->New.Data.hitAtt = MPShot;
    manage->New.Data.hitMask = MEnemy;

    manage->New.Data.X = x;
    manage->New.Data.Y = y;
    if (attack >= 25)
      manage->New.Data.Attack = 3;
    else
      manage->New.Data.Attack = 2;
    
    manage->New.Data.Speed = speed;
    manage->New.Data.Angle = angle;
    manage->New.Data.EnemyAtt = NullDel;
    manage->New.Data.Width = 10;
    manage->New.Data.Height = 26;

    manage->New.Data.Cnt[0] = x << 8;
    manage->New.Data.Cnt[1] = y << 8;
    manage->New.Data.Cnt[2] = icos(angle);
    manage->New.Data.Cnt[3] = isin(angle);

    manage->New.Data.Cnt[4] = 0;

    manage->New.Grp.image = PShot1Image;

    NewObj(MPShot,PlayerShotAct1,PlayerShotHit1,DrawImage);
}

void PlayerShot2(int x, int y, int speed, int angle)
{
    manage->New.Data.hitAtt = MPShot;
    manage->New.Data.hitMask = MEnemy;

    manage->New.Data.X = x;
    manage->New.Data.Y = y;
    manage->New.Data.Attack = 1;
    manage->New.Data.Speed = speed;
    manage->New.Data.Angle = angle;
    manage->New.Data.EnemyAtt = NullDel;
    manage->New.Data.Width = 12;
    manage->New.Data.Height = 16;

    manage->New.Data.Cnt[0] = x << 8;
    manage->New.Data.Cnt[1] = y << 8;
    manage->New.Data.Cnt[2] = icos(angle);
    manage->New.Data.Cnt[3] = isin(angle);

    manage->New.Data.Cnt[4] = 0;

    manage->New.Grp.image = PShot2Image;

    NewObj(MPShot,PlayerShotAct1,PlayerShotHit1,DrawImage);
}

DelAtt PlayerShotAct1(ObjData *my)
{
    if (my->Cnt[4] >= 1)
	return NullDel;

    /* 2^8 = 256 */
    my->Cnt[0] += my->Cnt[2]*my->Speed;
    my->X = my->Cnt[0] / 256;
    my->Cnt[1] += my->Cnt[3]*my->Speed;
    my->Y = my->Cnt[1] / 256;

    if ((my->X<0) || (my->X>FieldW))
	return NullDel;
    if ((my->Y<0) || (my->Y>FieldH))
	return NullDel;

    return NoneDel;
}

DelAtt PlayerShotHit1(ObjData *my, ObjData *your)
{
    if (my->Cnt[4] >= 1)
	return NullDel;

    my->Cnt[4]++;
    my->hitMask = 0;
    my->image = 1;
    return NoneDel;
}

void PlayerShot3(int x, int y, int inertX, int attack)
{
    manage->New.Data.hitAtt = MPShot;
    manage->New.Data.hitMask = MEnemy;

    manage->New.Data.X = x;
    manage->New.Data.Y = y;
    if (attack >= 22)
      manage->New.Data.Attack = 5;
    else if (attack >= 16)
      manage->New.Data.Attack = 4;
    else
      manage->New.Data.Attack = 3;
    manage->New.Data.EnemyAtt = NullDel;
    manage->New.Data.Width = 8;
    manage->New.Data.Height = 16;

    manage->New.Data.inertX = inertX;
    manage->New.Data.inertY = 5;
    manage->New.Data.Cnt[4] = 0;

    manage->New.Grp.image = PShot3Image;

    NewObj(MPShot,PlayerShotAct3,PlayerShotHit3,DrawImage);
}

DelAtt PlayerShotAct3(ObjData *my)
{
    if (my->Cnt[4] != 0)
    {
      /* delete the explosion */
	if (my->Cnt[4] >= 4)
	    return NullDel;
        /* no longer checks collision */
	if (my->Cnt[4] == 3)
          /*
	    my->Attack = 0;
          */
          my->hitMask = 0;
	if (my->Cnt[4] == 1)
	    my->image = 2;
	my->Cnt[4]++;
    }

    /* have not exploded yet */
    if (my->Cnt[4] == 0)
    {
	my->inertY++;
	my->X += my->inertX;
	my->Y -= my->inertY;
    }

    if ((my->X<0) || (my->X>FieldW))
	return NullDel;
    if ((my->Y<0) || (my->Y>FieldH))
	return NullDel;

    return NoneDel;
}

DelAtt PlayerShotHit3(ObjData *my, ObjData *your)
{
    if (my->Cnt[4] >= 4)
	return NullDel;
    else if (my->Cnt[4] == 0)
      /* explode */
    {
      my->Attack = 1;

	my->Width = 80;
	my->HarfW = my->Width / 2;
	
	my->Height = 80;
	my->HarfH = my->Height / 2;
	
	my->Cnt[4]++;
	my->image = 1;
	return NoneDel;
    }
    return NoneDel;
}

void PlayerLosePower(void)
{
  int i = 0;
  
  if (manage->player[0]->Data.Cnt[6] > 30)
    manage->player[0]->Data.Cnt[6] = 30;
  
  if (manage->player[0]->Data.Cnt[6] < manage->start_power + 10)
    return;
  else if (manage->player[0]->Data.Cnt[6] < manage->start_power + 25)
  {
    manage->player[0]->Data.Cnt[6] -= 5;
    i = 3;
  }
  else
  {
    manage->player[0]->Data.Cnt[6] -= 10;
    i = 6;
  }

  if (manage->player[0]->Data.Cnt[6] < 0)
    /* should not happen */
    manage->player[0]->Data.Cnt[6] = 0;

  for (; i >= 1; i--)
    /* the Y coordinate is intentionally subtracted */
    NewEnemy10(manage->player[0]->Data.X + integerrng() % 50 - 25,
               manage->player[0]->Data.Y - integerrng() % 20 - 10);
}
/* xsoldier, a shoot 'em up game with "not shooting" bonus
 * Copyright (C) 1997 Yuusuke HASHIMOTO <s945750@educ.info.kanagawa-u.ac.jp>
 * Copyright (C) 2002 Oohara Yuuma  <oohara@libra.interq.or.jp>
 *
 * This is a copyleft program.  See the file LICENSE for details.
 */
/* $Id: enemyshot.c,v 1.7 2002/04/29 16:54:59 oohara Exp $ */

#include <stdio.h>
#include <stdlib.h>

/*
#include <X11/Xlib.h>
#include <X11/xpm.h>
*/

#include "image.h"
#include "xsoldier.h"
#include "manage.h"
#include "common.h"
#include "enemyshot.h"
#include "callback.h"
#include "extern.h"

/* enemy shot */
void ShotToAngle(int x, int y, int angle, int speed)
{
    int i;

    if (manage->EnemyNum >= manage->EnemyMax)
        return;

    if (speed <= 0)
      speed = 1;
    
    for (i=1; i<manage->EnemyMax; i++)
    {
        if (manage->enemy[i]->Data.used == False)
	{
	    manage->EnemyShot.Data.X = x;
	    manage->EnemyShot.Data.Y = y;
	    manage->EnemyShot.Data.Angle = angle;
	    manage->EnemyShot.Data.Speed = speed;
	    manage->EnemyShot.Data.Cnt[0] = x << 8;
	    manage->EnemyShot.Data.Cnt[1] = y << 8;
	    manage->EnemyShot.Data.Cnt[2] = icos(angle);
	    manage->EnemyShot.Data.Cnt[3] = isin(angle);

	    manage->enemy[i]->Data    = manage->EnemyShot.Data;
	    manage->enemy[i]->Grp     = manage->EnemyShot.Grp;
            manage->enemy[i]->Action  = EnemyShotAct;
            manage->enemy[i]->Realize = DrawImage;
            manage->enemy[i]->Hit     = NullDelHit;

	    manage->EnemyNum++;
	    return;
	}
    }
}

void ShotToPoint(int x1, int y1, int x2, int y2, int speed)
{
    int i;
    int diffx = x2 - x1;
    int diffy = y2 - y1;
    int absx = abs(diffx);
    int absy = abs(diffy);

    if ( manage->EnemyNum >= manage->EnemyMax ){ return; }
    if ( speed <= 0 ){ speed = 1; }
    for (i=1; i<manage->EnemyMax; i++)
    {
        if (manage->enemy[i]->Data.used == False)
	{
	    manage->EnemyShot.Data.X = x1;
	    manage->EnemyShot.Data.Y = y1;
	    manage->EnemyShot.Data.Speed = speed;
	    manage->EnemyShot.Data.Cnt[0] = x1 << 8;
	    manage->EnemyShot.Data.Cnt[1] = y1 << 8;
	    if (absx >= absy)
	    {
		manage->EnemyShot.Data.Cnt[2] = 1 << 8;
		manage->EnemyShot.Data.Cnt[3] = (absy/(double)absx) * 256;
	    }
	    else
	    {
		manage->EnemyShot.Data.Cnt[2] = (absx/(double)absy) * 256;
		manage->EnemyShot.Data.Cnt[3] = 1 << 8;
	    }
	    if (diffx < 0)
		manage->EnemyShot.Data.Cnt[2] *= -1;
	    if (diffy < 0)
		manage->EnemyShot.Data.Cnt[3] *= -1;

	    manage->enemy[i]->Data    = manage->EnemyShot.Data;
	    manage->enemy[i]->Grp     = manage->EnemyShot.Grp;
            manage->enemy[i]->Action  = EnemyShotAct;
            manage->enemy[i]->Realize = DrawImage;
            manage->enemy[i]->Hit     = NullDelHit;

	    manage->EnemyNum++;
	    return;
	}
    }
}

DelAtt EnemyShotAct(ObjData *my)
{
  /* 2^8 = 256 */
    my->Cnt[0] += my->Cnt[2]*my->Speed;
    my->X = my->Cnt[0] / 256;
    my->Cnt[1] += my->Cnt[3]*my->Speed;
    my->Y = my->Cnt[1] / 256;

    my->Cnt[4]++;
    if (my->Cnt[4] >= 3)
    {
	my->Cnt[4] = 0;
	my->image++;
	if (my->image > 3)
	    my->image = 0;
    }

    if ((my->X < 0 - my->Width/2) || (my->X > FieldW + my->Width/2))
	return NullDel;
    if ((my->Y < 0 - my->Height/2) || (my->Y > FieldH + my->Height/2))
	return NullDel;

    return NoneDel;
}

int RingToAngle(int x, int y, int angle, int speed)
{
  if (speed <= 0) { speed = 1; }
  
    manage->New.Data.hitAtt = MEnemy;
    manage->New.Data.hitMask = MPlayer | MPShot;

    manage->New.Data.X = x;
    manage->New.Data.Y = y;
    manage->New.Data.HP = 1;
    manage->New.Data.Point = 0;
    manage->New.Data.Angle = angle;
    manage->New.Data.Speed = speed;
    manage->New.Data.Cnt[0] = x << 8;
    manage->New.Data.Cnt[1] = y << 8;
    manage->New.Data.Cnt[2] = icos(angle);
    manage->New.Data.Cnt[3] = isin(angle);
    manage->New.Data.Cnt[4] = 0;
    manage->New.Data.EnemyAtt = NullDel;
    manage->New.Data.Width = 28;
    manage->New.Data.Height = 28;

    manage->New.Grp.image = ERingImage;

    return NewObj(MEnemy,EnemyShotAct,DeleteHit,DrawImage);
}

int RingToPoint(int x1, int y1, int x2, int y2, int speed) {
    int diffx = x2 - x1;
    int diffy = y2 - y1;
    int absx = abs(diffx);
    int absy = abs(diffy);

    if (speed <= 0) { speed = 1; }
    manage->New.Data.hitAtt = MEnemy;
    manage->New.Data.hitMask = MPlayer | MPShot;

    manage->New.Data.X = x1;
    manage->New.Data.Y = y1;
    manage->New.Data.HP = 1;
    manage->New.Data.Point = 0;
    manage->New.Data.Speed = speed;
    manage->New.Data.Cnt[0] = x1 << 8;
    manage->New.Data.Cnt[1] = y1 << 8;

    if (absx >= absy)
    {
	manage->New.Data.Cnt[2] = 1 << 8;
	manage->New.Data.Cnt[3] = (absy/(double)absx) * 256;
    }
    else
    {
	manage->New.Data.Cnt[2] = (absx/(double)absy) * 256;
	manage->New.Data.Cnt[3] = 1 << 8;
    }
    if (diffx < 0)
	manage->New.Data.Cnt[2] *= -1;
    if (diffy < 0)
	manage->New.Data.Cnt[3] *= -1;

    manage->New.Data.Cnt[4] = 0;
    manage->New.Data.EnemyAtt = NullDel;
    manage->New.Data.Width = 28;
    manage->New.Data.Height = 28;

    manage->New.Grp.image = ERingImage;

    return NewObj(MEnemy,EnemyShotAct,DeleteHit,DrawImage);
}

int HomingShot(int x, int y, int ix, int iy)
{
    manage->New.Data.hitAtt = MEnemy;
    manage->New.Data.hitMask = MPlayer | MPShot;

    manage->New.Data.X = x;
    manage->New.Data.Y = y;
    manage->New.Data.HP = 1;
    manage->New.Data.Point = 0;
    manage->New.Data.EnemyAtt = NullDel;
    manage->New.Data.Width = 16;
    manage->New.Data.Height = 16;
    manage->New.Data.inertX = ix;
    manage->New.Data.inertY = iy;

    manage->New.Grp.image = EMissileImage;

    return NewObj(MEnemy,HomingAct,DeleteHit,DrawImage);
}

DelAtt HomingAct(ObjData *my)
{
    if (my->X < manage->player[0]->Data.X)
    {
	if (my->inertX < 15)
            my->inertX += 1;
    }
    else
    {
	if (my->inertX > -15)
            my->inertX -= 1;
    }    

    if (my->Y < manage->player[0]->Data.Y)
	my->inertY++;
    else
	my->inertY--;

    my->X += my->inertX;
    my->Y += my->inertY;

    my->image = GetDirection(0,0,my->inertX,my->inertY);

    if ((my->X < 0 - my->Width/2) || (my->X > FieldW + my->Width/2))
	return NullDel;
    if ((my->Y < 0 - my->Height/2) || (my->Y > FieldH + my->Height/2))
	return NullDel;

    return NoneDel;
}

int LaserShot(int x, int y, int speed)
{
  if (speed <= 0) { speed = 1; }

  manage->New.Data.hitAtt = MEShot;
  manage->New.Data.hitMask = MPlayer;

  manage->New.Data.X = x;
  manage->New.Data.Y = y;
  manage->New.Data.EnemyAtt = NullDel;
  manage->New.Data.Width = 7;
  manage->New.Data.Height = 50;
  manage->New.Data.Speed = speed;

  manage->New.Grp.image = ELaserImage;

  return NewObj(MEShot,EnemyLaserAct,NullHit,DrawImage);
}

DelAtt EnemyLaserAct(ObjData *my)
{
    my->Y += my->Speed;

    if ((my->X < 0 - my->Width/2) || (my->X > FieldW + my->Width/2))
	return NullDel;
    if ((my->Y < 0 - my->Height/2) || (my->Y > FieldH + my->Height/2))
	return NullDel;

    return NoneDel;
}

int BoundShot(int x, int y, int ix, int iy, int bound)
{
  if ( (ix == 0) && (iy == 0) ) { ix = 1; }

  manage->New.Data.hitAtt = MEnemy;
  manage->New.Data.hitMask = MPlayer | MPShot;

  manage->New.Data.X = x;
  manage->New.Data.Y = y;
  manage->New.Data.EnemyAtt = NullDel;
  manage->New.Data.Width = 28;
  manage->New.Data.Height = 28;
  manage->New.Data.inertX = ix;
  manage->New.Data.inertY = iy;

  manage->New.Data.Cnt[0] = 0;
  manage->New.Data.Cnt[1] = bound;

  manage->New.Grp.image = EBoundImage;

  return NewObj(MEShot,BoundShotAct,NullHit,DrawImage);
}

DelAtt BoundShotAct(ObjData *my)
{
  if (my->Cnt[0] <= my->Cnt[1])
  {
    if ((my->X+my->inertX>FieldW) || (my->X+my->inertX<0))
    {
      my->inertX = my->inertX*(-1);
      my->Cnt[0]++;
    }
    if ((my->Y+my->inertY>FieldH) || (my->Y+my->inertY<0))
    {
      my->inertY = my->inertY*(-1);
      my->Cnt[0]++;
    }
  }
    
    my->image++;
    if (my->image >= 8)
	my->image = 0;

    my->X += my->inertX;
    my->Y += my->inertY;

    if ((my->X < 0 - my->Width/2) || (my->X > FieldW + my->Width/2))
	return NullDel;
    if ((my->Y < 0 - my->Height/2) || (my->Y > FieldH + my->Height/2))
	return NullDel;

    return NoneDel;
}
/* xsoldier, a shoot 'em up game with "not shooting" bonus
 * Copyright (C) 1997 Yuusuke HASHIMOTO <s945750@educ.info.kanagawa-u.ac.jp>
 * Copyright (C) 2002 Oohara Yuuma  <oohara@libra.interq.or.jp>
 *
 * This is a copyleft program.  See the file LICENSE for details.
 */
/* $Id: enemy.c,v 1.16 2002/04/29 03:40:19 oohara Exp $ */


/* abs */
#include <stdlib.h>

/*
#include <X11/Xlib.h>
#include <X11/xpm.h>
*/

#include "image.h"
#include "xsoldier.h"
#include "manage.h"
#include "common.h"
#include "enemyshot.h"
#include "callback.h"
#include "enemy.h"
#include "extern.h"



/* definition of objects
 * - initialization function
 * - action function
 * - hit function
 *
 * initialization add action function and display/hit function to the table
 */

/* run straight ahead */
int NewEnemy1(int x, int y)
{
  /* attribute of this object */
    manage->New.Data.hitAtt = MEnemy;
    /* what objects should hit this object */
    manage->New.Data.hitMask = MPlayer | MPShot;

    manage->New.Data.HP = 1;
    manage->New.Data.Point = 100;
    manage->New.Data.EnemyAtt = ZakoDel;

    /* size for collision detection */
    manage->New.Data.Width = 32;
    manage->New.Data.Height = 32;

    manage->New.Data.X = x;
    manage->New.Data.Y = 1 - manage->New.Data.Height/2;

    manage->New.Data.Angle = 0;
    manage->New.Data.Speed = integerrng()%15 + 1;

    manage->New.Data.Cnt[0] = 0;

    /* pixmap for this object */
    manage->New.Grp.image = Enemy1Image;

    /* add action function and hit function to the table */
    return NewObj(MEnemy,EnemyAct1,EnemyHit1,DrawImage);
}

DelAtt EnemyAct1(ObjData *my)
{
  int i;
    if (manage->Loop > 1)
    {
      /* shoot in loop 2 or later */
	if ( my->shotTime >= ShotTiming ) {
          for (i =1; i< manage->Loop; i++) {
	    ShotToPoint(my->X,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y,5);
	    my->shotTime = my->startTime;
          }
	} else {
	    my->shotTime++;
        }
    }
    
    my->image = 4;
    my->Y += my->Speed;

    if ((my->X < 0 - my->Width/2) || (my->X > FieldW + my->Width/2))
	return NullDel;
    if ((my->Y < 0 - my->Height/2) || (my->Y > FieldH + my->Height/2))
	return NullDel;

    return NoneDel;
}

/* may give you an item */
DelAtt EnemyHit1(ObjData *my, ObjData *your)
{
  int temp = your->Attack;
    if (my->HP < your->Attack)
      temp = my->HP;

    my->HP -= temp;
    player->Rec[0].score += temp;

    if (my->HP <= 0)
    {
      player->Rec[0].score -= 1;

      /* always gives you a shot in loop 3 or later */
	if (manage->Loop > 2)
	    ShotToPoint(my->X,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y,5);

	if (integerrng()%100 < 20)
	    NewEnemy10(my->X,my->Y);
        NewBomb(my->X,my->Y);

        my->showDamegeTime = 0;
        return my->EnemyAtt;
    }
    else
    {
      my->showDamegeTime = 15;
      return NoneDel;
    }
    
}

/* codename "sine curve" (in fact it is a parabola) */
int NewEnemy2(int x, int y)
{

    manage->New.Data.hitAtt = MEnemy;
    manage->New.Data.hitMask = MPlayer | MPShot;

    manage->New.Data.HP = 1;
    manage->New.Data.Point = 100;
    manage->New.Data.EnemyAtt = ZakoDel;

    manage->New.Data.Width = 32;
    manage->New.Data.Height = 32;

    manage->New.Data.X = x;
    manage->New.Data.Y = 1 - manage->New.Data.Height/2;

    manage->New.Data.inertX = 0;
    manage->New.Data.inertY = 1;

    manage->New.Data.Angle = 0;
    manage->New.Data.Speed = 0;
    manage->New.Data.Cnt[0] = 0;
    /* range of x */
    manage->New.Data.Cnt[1] = integerrng()%3 + 5;
    if (x < FieldW/2)
	manage->New.Data.Cnt[2] = 0;
    else
	manage->New.Data.Cnt[2] = 1;

    manage->New.Data.Cnt[3] = 0;

    manage->New.Grp.image = Enemy3Image;

    return NewObj(MEnemy,EnemyAct2,DamageHit,DrawImage);
}

DelAtt EnemyAct2(ObjData *my)
{
    if (my->shotTime >= ShotTiming)
    {
	if (manage->Loop > 1)
	    LaserShot(my->X,my->Y,10);
        ShotToPoint(my->X,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y,4);
	my->shotTime = my->startTime;
    }
    else
	my->shotTime++;

    if (my->Cnt[2] == 0)
    {
        my->Cnt[3]++;

        if(my->Cnt[3] >= 2*my->Cnt[1])
            my->Cnt[2] = 1;
    }
    else if (my->Cnt[2] == 1)
    {
        my->Cnt[3]--;

        if(my->Cnt[3] <= -2*my->Cnt[1])
            my->Cnt[2] = 0;
    }

    my->Y += my->Cnt[1];
    my->X += my->Cnt[3];

    /* what number of pixmap we use */
    if (my->Cnt[3] > my->Cnt[1])
        my->image = 2;
    else if (my->Cnt[3] > 0)
        my->image = 3;
    else if (my->Cnt[3] < -my->Cnt[1])
        my->image = 6;
    else if(my->Cnt[3] < 0)
        my->image = 5;
    else
        my->image = 0;

    if ((my->Y < 0 - my->Height/2) || (my->Y > FieldH + my->Height/2))
	return NullDel;

    return NoneDel;
}

/* turns left/right */
int NewEnemy3(int x, int y)
{

    manage->New.Data.hitAtt = MEnemy;
    manage->New.Data.hitMask = MPlayer | MPShot;

    manage->New.Data.HP = 2;
    manage->New.Data.Point = 150;
    manage->New.Data.EnemyAtt = ZakoDel;

    manage->New.Data.Width = 32;
    manage->New.Data.Height = 32;

    manage->New.Data.X = x;
    manage->New.Data.Y = 1 - manage->New.Data.Height/2;

    manage->New.Data.Cnt[0] = 0;
    manage->New.Data.Cnt[1] = 0;
    manage->New.Data.Cnt[2] = 5;

    manage->New.Grp.image = Enemy1Image;
    return NewObj(MEnemy,EnemyAct3,EnemyHit1,DrawImage);
}

DelAtt EnemyAct3(ObjData *my)
{
    if (my->shotTime >= ShotTiming)
    {
        ShotToPoint(my->X,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y,4);
        my->shotTime = my->startTime;
    }
    else
        my->shotTime++;
    
    if (my->Cnt[0] == 0)
    {
	if (my->Y<manage->player[0]->Data.Y+10 && my->Y>manage->player[0]->Data.Y-10)
	{
	    if (my->X < manage->player[0]->Data.X)
            {
              my->Cnt[0] = 1;
              if (manage->Loop >2)
                BoundShot(my->X, my->Y, integerrng()%30-15, integerrng()%30-15, 3);
            }
            
	    if (my->X > manage->player[0]->Data.X)
            {
              my->Cnt[0] = 2;
              if (manage->Loop >2)
                BoundShot(my->X, my->Y, integerrng()%30-15, integerrng()%30-15, 3);
            }
	}
    }

    if (my->Cnt[0] == 0)
    {
	my->Y += 2 + manage->Loop * 3;
	my->image = 4;
    }
    else if (my->Cnt[0] == 1)
    {
	my->X += 15;
	my->image = 2;
    }
    else if (my->Cnt[0] == 2)
    {
	my->X -= 15;
	my->image = 6;
    }

    if ((my->X < 0 - my->Width/2) || (my->X > FieldW + my->Width/2))
	return NullDel;
    if ((my->Y < 0 - my->Height/2) || (my->Y > FieldH + my->Height/2))
	return NullDel;

    return NoneDel;
}

/* chases you */
int NewEnemy4(int x, int y)
{

    manage->New.Data.hitAtt = MEnemy;
    manage->New.Data.hitMask = MPlayer | MPShot;

    manage->New.Data.HP = 2;
    manage->New.Data.Point = 150;
    manage->New.Data.EnemyAtt = ZakoDel;

    manage->New.Data.Width = 32;
    manage->New.Data.Height = 32;

    manage->New.Data.X = x;
    manage->New.Data.Y = 1 - manage->New.Data.Height/2;

    manage->New.Data.inertX = 0;
    manage->New.Data.inertY = 0;

    manage->New.Grp.image = Enemy2Image;
    return NewObj(MEnemy,EnemyAct4,DamageHit,DrawImage);
}

DelAtt EnemyAct4(ObjData *my)
{
    if (my->shotTime >= ShotTiming)
    {
        ShotToPoint(my->X,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y,4);
        if (manage->Loop >1)
        {
          ShotToAngle(my->X, my->Y, 90, 4);
          ShotToAngle(my->X, my->Y, 90, 8);
          ShotToAngle(my->X, my->Y, 90, 12);
          ShotToAngle(my->X, my->Y, 90, 16);
        }
        
	my->shotTime = my->startTime;
    }
    else
	my->shotTime++;

    if (my->X < manage->player[0]->Data.X)
    {
      if (my->inertX <= 8)
        my->inertX++;
    }
    else if (my->X > manage->player[0]->Data.X)
    {
      if (my->inertX >= -8)
        my->inertX--;
    }

    if (my->Y < manage->player[0]->Data.Y)
    {
      if (my->inertY <= 8)
        my->inertY++;
    }
    else if (my->Y > manage->player[0]->Data.Y)
    {
      if (my->inertY >= -8)
        my->inertY--;
    }
    
    my->X += my->inertX;
    my->Y += my->inertY;

    my->image = GetDirection(my->X,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y);
    
    if ((my->X < 0 - my->Width/2) || (my->X > FieldW + my->Width/2))
	return NullDel;
    if ((my->Y < 0 - my->Height/2) || (my->Y > FieldH + my->Height/2))
	return NullDel;

    return NoneDel;
}

/* bounce on the left/right side of the window */
int NewEnemy5(int x, int y)
{

    manage->New.Data.hitAtt = MEnemy;
    manage->New.Data.hitMask = MPlayer | MPShot;

    if (x > FieldW/2)
    {
	manage->New.Data.X = FieldW;
	manage->New.Data.Cnt[0] = - (integerrng()%10 + 15);
    }
    else
    {
	manage->New.Data.X = 0;
	manage->New.Data.Cnt[0] = integerrng()%10 + 15;
    }

    manage->New.Data.HP = 9;
    manage->New.Data.Point = 200;
    manage->New.Data.EnemyAtt = ZakoDel;

    manage->New.Data.Width = 32;
    manage->New.Data.Height = 32;

    manage->New.Data.Y = 1 - manage->New.Data.Height/2;

    manage->New.Data.inertX = 0;
    manage->New.Data.inertY = 0;

    manage->New.Grp.image = Enemy5Image;
    return NewObj(MEnemy,EnemyAct5,EnemyHit1,DrawImage);
}

DelAtt EnemyAct5(ObjData *my)
{
    if (my->Cnt[0] < 0)
    {
	my->inertX++;
	if (abs(my->inertX+my->Cnt[0]) < 5)
	    my->image = 3;
	else
	    my->image = 2;
    }
    else
    {
	my->inertX--;
	if (abs(my->inertX+my->Cnt[0]) < 5)
	    my->image = 1;
	else
	    my->image = 0;
    }

    if (my->Cnt[0]+my->inertX == 0)
    {
	RingToPoint(my->X,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y,integerrng()%4+2);
	RingToAngle(my->X, my->Y, 45, 5);
	RingToAngle(my->X, my->Y, 90, 5);
	RingToAngle(my->X, my->Y, 135, 5);
	RingToPoint(my->X,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y,integerrng()%2+4);
	if (manage->Loop > 1)
	{
	    RingToPoint(my->X,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y,integerrng()%3+3);
	    RingToPoint(my->X,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y,integerrng()%4+4);
	}
    }

    my->X += (my->Cnt[0] + my->inertX);
    my->Y += 5;

    if ((my->X < 0) || (my->X > FieldW))
	my->inertX = 0;

    if ((my->Y < 0 - my->Height/2) || (my->Y > FieldH + my->Height/2))
	return NullDel;

    return NoneDel;
}

/* bounce on any side of the window */
int NewEnemy6(int x, int y)
{

    manage->New.Data.hitAtt = MEnemy;
    manage->New.Data.hitMask = MPlayer | MPShot;

    manage->New.Data.HP = 4;
    manage->New.Data.Point = 200;
    manage->New.Data.EnemyAtt = ZakoDel;

    manage->New.Data.Width = 32;
    manage->New.Data.Height = 32;

    manage->New.Data.X = x;
    manage->New.Data.Y = 1 - manage->New.Data.Height/2;

    if (x > FieldW/2)
	manage->New.Data.inertX = integerrng()%5 + 10;
    else
	manage->New.Data.inertX = integerrng()%5 - 15;

    manage->New.Data.inertY = integerrng()%10 + 5;

    manage->New.Data.Cnt[0] = 0;
    manage->New.Data.Cnt[1] = 8;
    /* "appeared" counter */
    manage->New.Data.Cnt[2] = 0;

    manage->New.Grp.image = Enemy4Image;
    return NewObj(MEnemy,EnemyAct6,DamageHit,DrawImage);
}

DelAtt EnemyAct6(ObjData *my)
{
    if (my->shotTime >= ShotTiming)
    {
	ShotToPoint(my->X,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y,integerrng()%4+ manage->Loop * 7 -5);
	my->shotTime = my->startTime;
    }
    else
	my->shotTime++;

    if ((my->Cnt[2] != 0) && (my->Cnt[0] <= my->Cnt[1]))
    {
      if ((my->X+my->inertX>FieldW) || (my->X+my->inertX<0))
      {
        my->inertX = my->inertX*(-1);
        my->Cnt[0]++;
      }
      if ((my->Y+my->inertY>FieldH) || (my->Y+my->inertY<0))
      {
        my->inertY = my->inertY*(-1);
        my->Cnt[0]++;
      }
    }
    
    my->image++;
    if (my->image > 7)
	my->image = 0;

    my->X += my->inertX;
    my->Y += my->inertY;

    if ((my->X < 0 - my->Width/2) || (my->X > FieldW + my->Width/2))
	return NullDel;
    if ((my->Y < 0 - my->Height/2) || (my->Y > FieldH + my->Height/2))
	return NullDel;

    if ((my->Cnt[2] == 0) && (my->X >= 0) && (my->X <= FieldW)
        && (my->Y >= 0) && (my->Y < FieldH))
      my->Cnt[2] = 1;
    
    return NoneDel;
}

/* appears from the lower side of the window */
int NewEnemy7(int x, int y)
{

    manage->New.Data.hitAtt = MEnemy;
    manage->New.Data.hitMask = MPlayer | MPShot;

    if (x > FieldW/2)
	manage->New.Data.X = FieldW -50;
    else
	manage->New.Data.X =         50;

    manage->New.Data.HP = 3;
    manage->New.Data.Point = 250;
    manage->New.Data.EnemyAtt = ZakoDel;

    manage->New.Data.Width = 32;
    manage->New.Data.Height = 32;

    manage->New.Data.Y = FieldH - 1 + manage->New.Data.Height/2;

    manage->New.Data.Cnt[1] = 0;

    manage->New.Grp.image = Enemy3Image;

    return NewObj(MEnemy,EnemyAct7,EnemyHit1,DrawImage);
}

DelAtt EnemyAct7(ObjData *my)
{
    if (manage->Loop > 1)
    {
	if (my->shotTime >= ShotTiming)
	{
	    HomingShot(my->X,my->Y,0,0);
	    my->shotTime = my->startTime;
	}
	else
	    my->shotTime++;
    }

    if (my->Cnt[1] > 20)
    {
	my->Y -= 9;
	my->image = 0;
    }
    else if (my->Y > FieldH-100)
    {
	my->Y -= 6;
	my->image = 0;
    }
    else
    {
	my->Cnt[1]++;
	my->image++;
	if (my->image > 7)
	    my->image = 0;
    }

    if (my->Cnt[1] == 20)
    {
        ShotToAngle(my->X, my->Y, 144, 10);
        ShotToAngle(my->X, my->Y, 180, 10);
        ShotToAngle(my->X, my->Y, 216, 10);
        ShotToAngle(my->X, my->Y, 252, 10);
        ShotToAngle(my->X, my->Y, 288, 10);
        ShotToAngle(my->X, my->Y, 324, 10);
        ShotToAngle(my->X, my->Y, 360, 10);
        ShotToAngle(my->X, my->Y, 36, 10);
    }

    if ((my->X < 0 - my->Width/2) || (my->X > FieldW + my->Width/2))
	return NullDel;
    if ((my->Y < 0 - my->Height/2) || (my->Y > FieldH + my->Height/2))
	return NullDel;

    return NoneDel;
}

/* run straight ahead and shoot homing missiles */
int NewEnemy8(int x, int y)
{

    manage->New.Data.hitAtt = MEnemy;
    manage->New.Data.hitMask = MPlayer | MPShot;

    manage->New.Data.HP = 15;
    manage->New.Data.Point = 500;
    manage->New.Data.EnemyAtt = ZakoDel;

    manage->New.Data.Width = 40;
    manage->New.Data.Height = 50;

    manage->New.Data.X = x;
    manage->New.Data.Y = 1 - manage->New.Data.Height/2;

    manage->New.Data.Angle = 0;
    manage->New.Data.Speed = integerrng()%5 + 1;

    manage->New.Grp.image = Enemy7Image;

    return NewObj(MEnemy,EnemyAct8,LargeDamageHit,DrawImage);
}

DelAtt EnemyAct8(ObjData *my)
{
    if (my->shotTime >= ShotTiming)
    {
        HomingShot(my->X+10,my->Y-20,-10,-5);
        HomingShot(my->X-10,my->Y-20,10,-5);

	if (manage->Loop > 1)
	{
	    LaserShot(my->X-15,my->Y-20,10);
	    LaserShot(my->X+15,my->Y-20,10);
	}
	my->shotTime = my->startTime;
    }
    else
	my->shotTime++;
    
    my->Y += my->Speed;
    
    if ((my->X < 0 - my->Width/2) || (my->X > FieldW + my->Width/2))
	return NullDel;
    if ((my->Y < 0 - my->Height/2) || (my->Y > FieldH + my->Height/2))
	return NullDel;

    return NoneDel;
}

/* "reversed" parabola */
int NewEnemy9(int x, int y)
{
    manage->New.Data.hitAtt = MEnemy;
    manage->New.Data.hitMask = MPlayer | MPShot;

    manage->New.Data.HP = 6;
    manage->New.Data.Point = 300;
    manage->New.Data.EnemyAtt = ZakoDel;

    manage->New.Data.Width = 32;
    manage->New.Data.Height = 32;

    manage->New.Data.X = x;
    manage->New.Data.Y = 1 - manage->New.Data.Height/2;

    if (x > FieldW/2)
	manage->New.Data.inertX = -(integerrng()%10+1);
    else
	manage->New.Data.inertX = integerrng()%10+1;
    manage->New.Data.inertY = integerrng()%10 + 15;

    manage->New.Data.Cnt[0] = 0;

    manage->New.Grp.image = Enemy6Image;
    return NewObj(MEnemy,EnemyAct9,DamageHit,DrawImage);
}

DelAtt EnemyAct9(ObjData *my)
{
    if (manage->Loop > 2)
    {
	if (my->shotTime >= ShotTiming)
	{
	    ShotToAngle(my->X, my->Y, 50, 25);
	    ShotToAngle(my->X, my->Y, 70, 25);
	    ShotToAngle(my->X, my->Y, 90, 25);
	    ShotToAngle(my->X, my->Y, 110, 25);
	    ShotToAngle(my->X, my->Y, 130, 25);
	    my->shotTime = my->startTime;
	}
	else
	    my->shotTime++;
    }

    if (my->inertY<0 && my->inertY%2)
        ShotToPoint(my->X,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y,integerrng()%3+3);
	
    my->inertY--;

    my->X += my->inertX;
    my->Y += my->inertY;

    if (my->inertY > 10)
	my->image = 0;
    else if (my->inertY > 5)
	my->image = 1;
    else if (my->inertY > 0)
	my->image = 2;
    else if (my->inertY > -5)
	my->image = 3;
    else if (my->inertY > -10)
	my->image = 4;
    else
	my->image = 5;

    if ((my->X < 0 - my->Width/2) || (my->X > FieldW + my->Width/2))
	return NullDel;
    if ((my->Y < 0 - my->Height/2) || (my->Y > FieldH + my->Height/2))
	return NullDel;

    return NoneDel;
}

/* power-up item */
int NewEnemy10(int x, int y)
{
    manage->New.Data.hitAtt = MItem;
    manage->New.Data.hitMask = MPlayer;

    manage->New.Data.X = x;
    manage->New.Data.Y = y;

    manage->New.Data.HP = 0;
    manage->New.Data.Point = 0;
    manage->New.Data.EnemyAtt = NoneDel;

    manage->New.Data.Width = 32;
    manage->New.Data.Height = 32;

    manage->New.Data.inertY = -10 - integerrng() % 10;

    /* item counter */
    manage->New.Data.Cnt[0] = 0;
    /* time counter */
    manage->New.Data.Cnt[1] = 0;
    /* max speed */
    manage->New.Data.Cnt[2] = integerrng()%6 + 1;
    if (integerrng()%100 < 30)
    {
	manage->New.Data.Cnt[3] = 1;
	manage->New.Data.Cnt[0] = 1;
    }
    else
	manage->New.Data.Cnt[3] = 0;

    manage->New.Grp.image = ItemImage;
    return NewObj(MEnemy,EnemyAct10,NullDelHit,DrawImage);
}

DelAtt EnemyAct10(ObjData *my)
{
    my->Cnt[1]++;

    if (my->inertY < my->Cnt[2])
	my->inertY++;
    my->Y += my->inertY;

    if (my->Cnt[3] == 1)
    {
	if (my->Cnt[1]%25 == 0)
	{
	    if (my->Cnt[0] < 3)
		my->Cnt[0]++;
	    else
		my->Cnt[0] = 1;
	}
    }

    my->image = my->Cnt[0];

    if ((my->X < 0 - my->Width/2) || (my->X > FieldW + my->Width/2))
	return NullDel;
    if (my->Y > FieldH + my->Height/2)
	return NullDel;

    return NoneDel;
}
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

#include <X11/Xlib.h>
#include <X11/xpm.h>

#include "image.h"
#include "xsoldier.h"
#include "common.h"
#include "extern.h"
/* ShotToPoint */
#include "enemyshot.h"
#include "graphic.h"

#include "callback.h"

/* action */
/* do nothing */
DelAtt NullAct(ObjData *my)
{
    return NoneDel;
}


/* hit */
/* nothing can hit me, I am immutable */
DelAtt NullHit(ObjData *my, ObjData *your)
{
    return NoneDel;
}

/* simply die */
DelAtt NullDelHit(ObjData *my, ObjData *your)
{
    return NullDel;
}

/* die with explosion */
DelAtt DeleteHit(ObjData *my, ObjData *your)
{
    NewBomb(my->X,my->Y);
    return my->EnemyAtt;
}

/* deal damage, explode if dead */
DelAtt DamageHit(ObjData *my, ObjData *your)
{
  int temp = your->Attack;
    if (my->HP < your->Attack)
      temp = my->HP;

    my->HP -= temp;
   player->Rec[0].score += temp;
    if (my->HP <= 0)
    {
      player->Rec[0].score -= 1;
	if (manage->Loop > 2)
	    ShotToPoint(my->X,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y,5);
	NewBomb(my->X,my->Y);

        my->showDamegeTime = 0;
	return my->EnemyAtt;
    }
    else
    {
      my->showDamegeTime = 15; 
      return NoneDel;
    }
    
}

/* same above, but with big explosion */
DelAtt LargeDamageHit(ObjData *my, ObjData *your)
{
  int temp = your->Attack;
    if (my->HP < your->Attack)
      temp = my->HP;

    my->HP -= temp;
   player->Rec[0].score += temp;

    if (my->HP <= 0)
    {
      player->Rec[0].score -= 1;
	if (manage->Loop > 2)
	    ShotToPoint(my->X,my->Y,manage->player[0]->Data.X,manage->player[0]->Data.Y,5);
	NewLargeBomb(my->X,my->Y);

        my->showDamegeTime = 0;
	return my->EnemyAtt;
    }
    else
    {
      my->showDamegeTime = 15;      
      return NoneDel;
    }
}

/* display */
void NullReal(ObjData *my, GrpData *grp){ return; }

/* rectangle-drawing function for collision-detection debug */
void DrawRec(ObjData *my, GrpData *grp) {
  XDrawRectangle(dpy,WorkPixmap,FillGC,my->X-my->HarfW,my->Y-my->HarfH,my->Width,my->Height);
  return;
}

/* display pixmap */
void DrawImage(ObjData *my, GrpData *grp) {
    PutImage( grp->image[my->image],my->X - grp->HarfW, my->Y - grp->HarfH);
    return;
}
/* xsoldier, a shoot 'em up game with "not shooting" bonus
 * Copyright (C) 1997 Yuusuke HASHIMOTO <s945750@educ.info.kanagawa-u.ac.jp>
 * Copyright (C) 2002 Oohara Yuuma  <oohara@libra.interq.or.jp>
 *
 * This is a copyleft program.  See the file LICENSE for details.
 */
/* $Id: common.c,v 1.4 2002/04/29 03:38:41 oohara Exp $ */

/*
#include <X11/Xlib.h>
#include <X11/xpm.h>
*/
/* abs */
#include <stdlib.h>

#include "image.h"
#include "xsoldier.h"
#include "common.h"
#include "callback.h"
#include "extern.h"

int integerrng() { return lrand48(); }

void NewBomb(int x, int y) {
    int i;

    if (manage->EnemyNum >= manage->EnemyMax)
        return;

    for (i=1; i<manage->EnemyMax; i++) {
        if (manage->enemy[i]->Data.used == False)
	{
	    manage->Bomb.Data.X = x;
	    manage->Bomb.Data.Y = y;

	    manage->enemy[i]->Data    = manage->Bomb.Data;
	    manage->enemy[i]->Grp     = manage->Bomb.Grp;
            manage->enemy[i]->Action  = BombAct;
            manage->enemy[i]->Realize = DrawImage;
            manage->enemy[i]->Hit     = NullHit;

	    manage->EnemyNum++;
	    return;
	}
    }
}

void NewLargeBomb(int x, int y)
{
    int i;

    if (manage->EnemyNum >= manage->EnemyMax)
        return;

    for (i=1; i<manage->EnemyMax; i++)
    {
        if (manage->enemy[i]->Data.used == False)
	{
	    manage->LargeBomb.Data.X = x;
	    manage->LargeBomb.Data.Y = y;

	    manage->enemy[i]->Data    = manage->LargeBomb.Data;
	    manage->enemy[i]->Grp     = manage->LargeBomb.Grp;
            manage->enemy[i]->Action  = BombAct;
            manage->enemy[i]->Realize = DrawImage;
            manage->enemy[i]->Hit     = NullHit;

	    manage->EnemyNum++;
	    return;
	}
    }
}

DelAtt BombAct(ObjData *my)
{
    my->image = my->Cnt[0];
    my->Cnt[0]++;
    if (my->Cnt[0] > 5)
	return NullDel;
    return NoneDel;
}

int GetDirection(int mx, int my, int sx, int sy)
{
    static double hi;
    static int uw;
    static int uh;
    static int h;
    static int w;

    uw = abs(sx-mx);
    uh = abs(sy-my);
    h = sy-my;
    w = sx-mx;

    if (!uw) return (uh>0)?4:0;
    if (!uh) return (uw>0)?2:6;

    hi = (double)uh/uw;
    if (hi < 0.42)
        return (w > 0) ? 2: 6;
    else if (hi > 2.42)
        return (h > 0) ? 4: 0;
    else
    {
        return (w>0)?((h>0)?3:1):((h>0)?5:7);
        /***
        if (w > 0 && h > 0) return 3;
        if (w > 0 && h < 0) return 1;
        if (w < 0 && h > 0) return 5;
        if (w < 0 && h < 0) return 7;
        ***/
    }
}

static double dsin_table[] = {
  0.0000,
  0.0175,  0.0349,  0.0523,  0.0698,  0.0872,  0.1045,  0.1219,  0.1392,  0.1564,  0.1736,
  0.1908,  0.2079,  0.2250,  0.2419,  0.2588,  0.2756,  0.2924,  0.3090,  0.3256,  0.3420,
  0.3584,  0.3746,  0.3907,  0.4067,  0.4226,  0.4384,  0.4540,  0.4695,  0.4848,  0.5000,
  0.5150,  0.5299,  0.5446,  0.5592,  0.5736,  0.5878,  0.6018,  0.6157,  0.6293,  0.6428,
  0.6561,  0.6691,  0.6820,  0.6947,  0.7071,  0.7193,  0.7314,  0.7431,  0.7547,  0.7660,
  0.7771,  0.7880,  0.7986,  0.8090,  0.8192,  0.8290,  0.8387,  0.8480,  0.8572,  0.8660,
  0.8746,  0.8829,  0.8910,  0.8988,  0.9063,  0.9135,  0.9205,  0.9272,  0.9336,  0.9397,
  0.9455,  0.9511,  0.9563,  0.9613,  0.9659,  0.9703,  0.9744,  0.9781,  0.9816,  0.9848,
  0.9877,  0.9903,  0.9925,  0.9945,  0.9962,  0.9976,  0.9986,  0.9994,  0.9998,  1.0000,
  0.9998,  0.9994,  0.9986,  0.9976,  0.9962,  0.9945,  0.9925,  0.9903,  0.9877,  0.9848,
  0.9816,  0.9781,  0.9744,  0.9703,  0.9659,  0.9613,  0.9563,  0.9511,  0.9455,  0.9397,
  0.9336,  0.9272,  0.9205,  0.9135,  0.9063,  0.8988,  0.8910,  0.8829,  0.8746,  0.8660,
  0.8572,  0.8480,  0.8387,  0.8290,  0.8192,  0.8090,  0.7986,  0.7880,  0.7771,  0.7660,
  0.7547,  0.7431,  0.7314,  0.7193,  0.7071,  0.6947,  0.6820,  0.6691,  0.6561,  0.6428,
  0.6293,  0.6157,  0.6018,  0.5878,  0.5736,  0.5592,  0.5446,  0.5299,  0.5150,  0.5000,
  0.4848,  0.4695,  0.4540,  0.4384,  0.4226,  0.4067,  0.3907,  0.3746,  0.3584,  0.3420,
  0.3256,  0.3090,  0.2924,  0.2756,  0.2588,  0.2419,  0.2250,  0.2079,  0.1908,  0.1736,
  0.1564,  0.1392,  0.1219,  0.1045,  0.0872,  0.0698,  0.0523,  0.0349,  0.0175,  0.0000,
 -0.0175, -0.0349, -0.0523, -0.0698, -0.0872, -0.1045, -0.1219, -0.1392, -0.1564, -0.1736,
 -0.1908, -0.2079, -0.2250, -0.2419, -0.2588, -0.2756, -0.2924, -0.3090, -0.3256, -0.3420,
 -0.3584, -0.3746, -0.3907, -0.4067, -0.4226, -0.4384, -0.4540, -0.4695, -0.4848, -0.5000,
 -0.5150, -0.5299, -0.5446, -0.5592, -0.5736, -0.5878, -0.6018, -0.6157, -0.6293, -0.6428,
 -0.6561, -0.6691, -0.6820, -0.6947, -0.7071, -0.7193, -0.7314, -0.7431, -0.7547, -0.7660,
 -0.7771, -0.7880, -0.7986, -0.8090, -0.8192, -0.8290, -0.8387, -0.8480, -0.8572, -0.8660,
 -0.8746, -0.8829, -0.8910, -0.8988, -0.9063, -0.9135, -0.9205, -0.9272, -0.9336, -0.9397,
 -0.9455, -0.9511, -0.9563, -0.9613, -0.9659, -0.9703, -0.9744, -0.9781, -0.9816, -0.9848,
 -0.9877, -0.9903, -0.9925, -0.9945, -0.9962, -0.9976, -0.9986, -0.9994, -0.9998, -1.0000,
 -0.9998, -0.9994, -0.9986, -0.9976, -0.9962, -0.9945, -0.9925, -0.9903, -0.9877, -0.9848,
 -0.9816, -0.9781, -0.9744, -0.9703, -0.9659, -0.9613, -0.9563, -0.9511, -0.9455, -0.9397,
 -0.9336, -0.9272, -0.9205, -0.9135, -0.9063, -0.8988, -0.8910, -0.8829, -0.8746, -0.8660,
 -0.8572, -0.8480, -0.8387, -0.8290, -0.8192, -0.8090, -0.7986, -0.7880, -0.7771, -0.7660,
 -0.7547, -0.7431, -0.7314, -0.7193, -0.7071, -0.6947, -0.6820, -0.6691, -0.6561, -0.6428,
 -0.6293, -0.6157, -0.6018, -0.5878, -0.5736, -0.5592, -0.5446, -0.5299, -0.5150, -0.5000,
 -0.4848, -0.4695, -0.4540, -0.4384, -0.4226, -0.4067, -0.3907, -0.3746, -0.3584, -0.3420,
 -0.3256, -0.3090, -0.2924, -0.2756, -0.2588, -0.2419, -0.2250, -0.2079, -0.1908, -0.1736,
 -0.1564, -0.1392, -0.1219, -0.1045, -0.0872, -0.0698, -0.0523, -0.0349, -0.0175, -0.0000,
};

static int isin_table[] = {
    0,
    4,    8,   13,   17,   22,   26,   31,   35,   40,   44,
   48,   53,   57,   61,   66,   70,   74,   79,   83,   87,
   91,   95,  100,  104,  108,  112,  116,  120,  124,  127,
  131,  135,  139,  143,  146,  150,  154,  157,  161,  164,
  167,  171,  174,  177,  181,  184,  187,  190,  193,  196,
  198,  201,  204,  207,  209,  212,  214,  217,  219,  221,
  223,  226,  228,  230,  232,  233,  235,  237,  238,  240,
  242,  243,  244,  246,  247,  248,  249,  250,  251,  252,
  252,  253,  254,  254,  255,  255,  255,  255,  255,  256,
  255,  255,  255,  255,  255,  254,  254,  253,  252,  252,
  251,  250,  249,  248,  247,  246,  244,  243,  242,  240,
  238,  237,  235,  233,  232,  230,  228,  226,  223,  221,
  219,  217,  214,  212,  209,  207,  204,  201,  198,  196,
  193,  190,  187,  184,  181,  177,  174,  171,  167,  164,
  161,  157,  154,  150,  146,  143,  139,  135,  131,  127,
  124,  120,  116,  112,  108,  104,  100,   95,   91,   87,
   83,   79,   74,   70,   66,   61,   57,   53,   48,   44,
   40,   35,   31,   26,   22,   17,   13,    8,    4,    0,
   -4,   -8,  -13,  -17,  -22,  -26,  -31,  -35,  -40,  -44,
  -48,  -53,  -57,  -61,  -66,  -70,  -74,  -79,  -83,  -87,
  -91,  -95, -100, -104, -108, -112, -116, -120, -124, -127,
 -131, -135, -139, -143, -146, -150, -154, -157, -161, -164,
 -167, -171, -174, -177, -181, -184, -187, -190, -193, -196,
 -198, -201, -204, -207, -209, -212, -214, -217, -219, -221,
 -223, -226, -228, -230, -232, -233, -235, -237, -238, -240,
 -242, -243, -244, -246, -247, -248, -249, -250, -251, -252,
 -252, -253, -254, -254, -255, -255, -255, -255, -255, -256,
 -255, -255, -255, -255, -255, -254, -254, -253, -252, -252,
 -251, -250, -249, -248, -247, -246, -244, -243, -242, -240,
 -238, -237, -235, -233, -232, -230, -228, -226, -223, -221,
 -219, -217, -214, -212, -209, -207, -204, -201, -198, -196,
 -193, -190, -187, -184, -181, -177, -174, -171, -167, -164,
 -161, -157, -154, -150, -146, -143, -139, -135, -131, -128,
 -124, -120, -116, -112, -108, -104, -100,  -95,  -91,  -87,
  -83,  -79,  -74,  -70,  -66,  -61,  -57,  -53,  -48,  -44,
  -40,  -35,  -31,  -26,  -22,  -17,  -13,   -8,   -4,    0,
};

/* table-based approximate sine */
double dsin(int theta)
{
    while(theta > 360)
	theta -= 360;

    return (dsin_table[theta]);
}

/* returns 256 * sin(theta) */
int isin(int theta)
{
    while(theta > 360)
	theta -= 360;

    return (isin_table[theta]);
}
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

