
#include <config.h>

/* strncpy */
#include <string.h>
#include <malloc.h>

#include "image.h"
#include "xsoldier.h"
#include "manage.h"
#include "common.h"
#include "callback.h"
#include "extern.h"

#include "enemyshot.h"

CharManage *NewManage(int playerMax, int enemyMax) {
    CharManage *New;
    int i;

    New = (CharManage *)malloc(sizeof(CharManage));

    New->player = (CharObj **)malloc(sizeof(CharObj *) * playerMax);
    New->enemy = (CharObj **)malloc(sizeof(CharObj *) * enemyMax);

    New->PlayerMax = playerMax;
    New->PlayerNum = 0;
    New->EnemyMax = enemyMax;
    New->EnemyNum = 0;

    New->Level = 0;
    New->Stage = 1;
    New->Loop = 1;
    New->Appear = 0;

    New->StageEnemy = 0;
    New->StageShotDown = 0;
    New->ZakoApp = True;
    New->BossApp = False;
    New->BossKill = False;

    New->BossTime = 3000;
    New->flag_maxlevel = False;
    New->flag_nopausemessage = False;
    New->program_should_quit = False;

    for (i=0; i<New->PlayerMax; i++)
    {
	New->player[i] = (CharObj *)malloc(sizeof(CharObj));
	New->player[i]->Data.used = False;
	New->player[i]->Action    = NullAct;
	New->player[i]->Realize   = NullReal;
	New->player[i]->Hit       = NullHit;
    }

    for (i=0; i<New->EnemyMax; i++)
    {
	New->enemy[i] = (CharObj *)malloc(sizeof(CharObj));
	New->enemy[i]->Data.used = False;
	New->enemy[i]->Action    = NullAct;
	New->enemy[i]->Realize   = NullReal;
	New->enemy[i]->Hit       = NullHit;
    }

    /* frequently used objects */

    /* enemy shot */
    New->EnemyShot.Data.used = True;
    New->EnemyShot.Data.kill = False;
    New->EnemyShot.Data.hitAtt = MEShot;
    New->EnemyShot.Data.hitMask = MPlayer;
    New->EnemyShot.Data.EnemyAtt = NullDel;
    New->EnemyShot.Data.Width = 8;
    New->EnemyShot.Data.Height = 8;
    New->EnemyShot.Data.HarfW = New->EnemyShot.Data.Width/2;
    New->EnemyShot.Data.HarfH = New->EnemyShot.Data.Height/2;
    New->EnemyShot.Data.image = 0;
    New->EnemyShot.Data.Cnt[4] = 0; /* image counter */
    New->EnemyShot.Grp.image = EShotImage;
    New->EnemyShot.Grp.Width = New->EnemyShot.Grp.image[0]->width;
    New->EnemyShot.Grp.Height = New->EnemyShot.Grp.image[0]->height;
    New->EnemyShot.Grp.HarfW = New->EnemyShot.Grp.Width/2;
    New->EnemyShot.Grp.HarfH = New->EnemyShot.Grp.Height/2;

    /* small explosion */
    New->Bomb.Data.used = True;
    New->Bomb.Data.kill = False;
    New->Bomb.Data.hitAtt = 0;
    New->Bomb.Data.hitMask = 0;
    New->Bomb.Data.Width = 0;
    New->Bomb.Data.Height = 0;
    New->Bomb.Data.HarfW = New->Bomb.Data.Width/2;
    New->Bomb.Data.HarfH = New->Bomb.Data.Height/2;
    New->Bomb.Data.image = 0;
    New->Bomb.Data.Cnt[0] = 0;
    New->Bomb.Grp.image = BombImage;
    New->Bomb.Grp.Width = New->Bomb.Grp.image[0]->width;
    New->Bomb.Grp.Height = New->Bomb.Grp.image[0]->height;
    New->Bomb.Grp.HarfW = New->Bomb.Grp.Width/2;
    New->Bomb.Grp.HarfH = New->Bomb.Grp.Height/2;

    /* large explosion */
    New->LargeBomb.Data.used = True;
    New->LargeBomb.Data.kill = False;
    New->LargeBomb.Data.hitAtt = 0;
    New->LargeBomb.Data.hitMask = 0;
    New->LargeBomb.Data.Width = 0;
    New->LargeBomb.Data.Height = 0;
    New->LargeBomb.Data.HarfW = New->LargeBomb.Data.Width/2;
    New->LargeBomb.Data.HarfH = New->LargeBomb.Data.Height/2;
    New->LargeBomb.Data.image = 0;
    New->LargeBomb.Data.Cnt[0] = 0;
    New->LargeBomb.Grp.image = LargeBombImage;
    New->LargeBomb.Grp.Width = New->LargeBomb.Grp.image[0]->width;
    New->LargeBomb.Grp.Height = New->LargeBomb.Grp.image[0]->height;
    New->LargeBomb.Grp.HarfW = New->LargeBomb.Grp.Width/2;
    New->LargeBomb.Grp.HarfH = New->LargeBomb.Grp.Height/2;

    return New;
}

/* initialize manage at the beginning of stage */
void ClearManage(CharManage *manage_temp) {
  manage -> Appear        = ( manage->Stage > MaxStage ) ? -100 : -50;
  manage -> ZakoApp       = True;
  manage -> BossApp       = False;
  manage -> BossKill      = False;
  manage -> showShootDown = 1;
  manage -> StageEnemy    = 0;
  manage -> StageShotDown = 0;

  for ( int i=0; i < manage->EnemyMax; i++ ) {
    if (manage->enemy[i]->Data.used == True) {
      NewLargeBomb(manage->enemy[i]->Data.X,manage->enemy[i]->Data.Y);
      DelObj(manage->enemy[i]);
    }
  }
}

/* initialize manage at the beginning of game */
void ResetManage(CharManage *manage_temp)
{
    int i;

    manage->Level = 10;
    if (manage->Level > MaxLevel)
	manage->Level = MaxLevel;
    if (manage->Level < 0)
      manage->Level = 0;

    if (manage->flag_maxlevel == True)
      manage->Level = MaxLevel;

    manage->Appear = - 50;
    manage->ZakoApp = True;
    manage->BossApp = False;
    manage->BossKill = False;

    manage->StageEnemy = 0;
    manage->StageShotDown = 0;
    manage->BossTime = 3000;

    manage->showShootDown = 0;

    for (i=0; i<manage->PlayerMax; i++)
    {
	if (manage->player[i]->Data.used == True)
	    DelObj(manage->player[i]);
    }

    for (i=0; i<manage->EnemyMax; i++)
    {
	if (manage->enemy[i]->Data.used == True)
	    DelObj(manage->enemy[i]);
    }

    for (i=0; i<manage->PlayerMax; i++)
      manage->player[i]->Data.used = False;

    for (i=0; i<manage->EnemyMax; i++)
      manage->enemy[i]->Data.used = False;

    manage->PlayerNum = 0;
    manage->EnemyNum = 0;
}

void DeleteManage(CharManage *Del)
{
    int i;

    for (i=0; i<Del->PlayerMax; i++)
	free(Del->player[i]);
    for (i=0; i<Del->EnemyMax; i++)
	free(Del->enemy[i]);

    free(Del->player);
    free(Del->enemy);
    free(Del);
}

int NewObj(int mask,
	   DelAtt (*action)(ObjData *my),
	   DelAtt (*hit)(ObjData *my, ObjData *your),
	   void (*realize)(ObjData *my, GrpData *grp))
{
    int i;

    if (mask & (MPlayer|MPShot))
    {
	if (manage->PlayerNum >= manage->PlayerMax)
          /* no more room for an object */
	    return -1;
	for (i=1; i<manage->PlayerMax; i++)
	{
	    if (manage->player[i]->Data.used == False)
	    {
		manage->player[i]->Data    = manage->New.Data;
		manage->player[i]->Grp     = manage->New.Grp;
		manage->player[i]->Action  = action;
		manage->player[i]->Realize = realize;
		manage->player[i]->Hit     = hit;
		manage->player[i]->Data.used = True;
		manage->player[i]->Data.kill = False;
		manage->player[i]->Data.HarfW = manage->player[i]->Data.Width / 2;
		manage->player[i]->Data.HarfH = manage->player[i]->Data.Height / 2;

		manage->player[i]->Data.image = 0;
		manage->player[i]->Grp.Width = manage->player[i]->Grp.image[0]->width;
		manage->player[i]->Grp.Height = manage->player[i]->Grp.image[0]->height;
		manage->player[i]->Grp.HarfW = manage->player[i]->Grp.Width /2;
		manage->player[i]->Grp.HarfH = manage->player[i]->Grp.Height /2;
                manage->player[i]->Data.notShootingTime = 5;
		manage->PlayerNum++;
		return i;
	    }
	}
    }
    else
    {
	if (manage->EnemyNum >= manage->EnemyMax)
          /* no more room for an object */
	    return -1;

        /* 0 is reserved for end-of-stage boss */
	if (manage->New.Data.EnemyAtt == BossDel)
	    i = 0;
	else
	    i = 1;
	for ( ; i<manage->EnemyMax; i++)
	{
	    if (manage->enemy[i]->Data.used == False)
	    {
		manage->enemy[i]->Data    = manage->New.Data;
		manage->enemy[i]->Grp     = manage->New.Grp;
		manage->enemy[i]->Action  = action;
		manage->enemy[i]->Realize = realize;
		manage->enemy[i]->Hit     = hit;
		manage->enemy[i]->Data.used = True;
		manage->enemy[i]->Data.kill = False;
		manage->enemy[i]->Data.HarfW = manage->enemy[i]->Data.Width / 2;
		manage->enemy[i]->Data.HarfH = manage->enemy[i]->Data.Height / 2;
		manage->enemy[i]->Data.startTime = manage->Level;
		manage->enemy[i]->Data.shotTime = (ShotTiming + manage->Level) / 2;
		manage->enemy[i]->Data.image = 0;
		manage->enemy[i]->Grp.Width = manage->enemy[i]->Grp.image[0]->width;
		manage->enemy[i]->Grp.Height = manage->enemy[i]->Grp.image[0]->height;
		manage->enemy[i]->Grp.HarfW = manage->enemy[i]->Grp.Width / 2;
		manage->enemy[i]->Grp.HarfH = manage->enemy[i]->Grp.Height / 2;

		manage->enemy[i]->Data.showDamegeTime = 0;

		manage->EnemyNum++;
		return i;
	    }
	}
    }
    /* should not reach here */
    return -1;
}

void DelObj(CharObj *del)
{
    if (del->Data.used == True)
    {
	del->Data.used = False;
	if (del->Data.hitAtt & (MPlayer|MPShot))
	    manage->PlayerNum--;
	else
	    manage->EnemyNum--;
    }
}

PlayerData *NewPlayerData(void) {
    PlayerData *New;
    New = (PlayerData *)malloc(sizeof(PlayerData));
    strcpy(New->Rec[0].name,"noname");
    New->Rec[0].score = 0;
    New->Rec[0].stage = 0;
    New->Rec[0].loop = 0;
    return New;
}

/* clear all enemy shots */
void ClearEnemyShotManage(CharManage *manage_temp) {
    for (int i=0; i<manage->EnemyMax; i++) {
      if (manage->enemy[i]->Data.used == True)
	if ((manage->enemy[i]->Action == EnemyShotAct)
            || (manage->enemy[i]->Action == HomingAct)
            || (manage->enemy[i]->Action == EnemyLaserAct)
            || (manage->enemy[i]->Action == BoundShotAct))
	{
          /* small is beautiful */
          NewBomb(manage->enemy[i]->Data.X,manage->enemy[i]->Data.Y);
          DelObj(manage->enemy[i]);
	}
    }
}
