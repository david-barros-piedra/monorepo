#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_
extern int graphic_init(void);
extern int clear_window(void);
extern int redraw_window(void);
extern int graphic_finish(void);
extern int draw_string(int x, int y, const char *string, int length);
extern int draw_char(int x, int y, int c);

#endif /* _GRAPHIC_H_ */
