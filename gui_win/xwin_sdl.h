/*
 * File name: xwin_sdl.h
 * Date:      2015/06/18 14:37
 * Author:    Jan Faigl && Jakub Sukdol
 */

#ifndef __XWIN_SDL_H__
#define __XWIN_SDL_H__

#include <SDL2/SDL_image.h>

#include <SDL2/SDL.h>
#define BUTTON_WIN_W 120

enum {
    REDRAW_ANIM,
    REDRAW_C_IM_P,
    REDRAW_C_IM_M,
    REDRAW_C_RE_P,
    REDRAW_C_RE_M
} ;

int xwin_init(int w, int h);
void xwin_close();
void xwin_redraw(int w, int h, unsigned char *img);
void xwin_poll_events(void);
void xwin_resize(int width, int height);
void xwin_redraw_button(int button, bool anim, bool anim_change);

#endif

/* end of xwin_sdl.h */
