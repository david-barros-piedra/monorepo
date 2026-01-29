
#if !defined _COMMON_H_
#define _COMMON_H_

int integerrng(void) ;
extern void NewBomb(int x, int y);
extern void NewLargeBomb(int x, int y);
extern DelAtt BombAct(ObjData *my);
extern int GetDirection(int mx, int my, int sx, int sy);

double dsin(int theta);
int isin(int theta);
#define dcos(i) dsin(i+90)
#define icos(i) isin(i+90)


#endif
