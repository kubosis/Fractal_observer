/*
 * File name: xwin_sdl.c
 * Date:      2015/06/18 14:37
 * Author:    Jan Faigl && Jakub Sukdol
 */

#include <assert.h>

#include <time.h>
#include "xwin_sdl.h"
#include "../utils/utils.h"
#include "icon.h"
#include "gui_buttons.h"

static SDL_Window *win = NULL;

int xwin_init(int w, int h)
{
      int r;
      r = SDL_Init(SDL_INIT_VIDEO);
      assert(win == NULL);
      win = SDL_CreateWindow("PRG Semester Project", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w + BUTTON_WIN_W, h,  SDL_WINDOW_RESIZABLE);
      assert(win != NULL);
      SDL_SetWindowTitle(win, "FRACTAL OBSERVER");
      SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(icon, 64, 48, 24, 64 * 3, 0xff, 0xff00, 0xff0000, 0x0000);
      SDL_SetWindowIcon(win, surface);

      SDL_FreeSurface(surface);
   return r;
}

void xwin_close()
{
   assert(win != NULL);
   SDL_DestroyWindow(win);
   SDL_Quit();
}

void xwin_redraw(int w, int h, unsigned char *img)
{
   SDL_Surface *scr = SDL_GetWindowSurface(win);
   for(int y = 0; y < scr->h; ++y) {
      for(int x = 0; x < scr->w - BUTTON_WIN_W; ++x) {
         const int idx = (y * (scr->w) + x) * scr->format->BytesPerPixel;
         Uint8 *px = (Uint8*)scr->pixels + idx;
         *(px + scr->format->Rshift / 8) = *(img++);
         *(px + scr->format->Gshift / 8) = *(img++);
         *(px + scr->format->Bshift / 8) = *(img++);
      }
   }
   SDL_UpdateWindowSurface(win);
}

void xwin_poll_events(void) 
{
   SDL_Event event;
   while (SDL_PollEvent(&event));
}

void xwin_resize(int width, int height) 
{
   SDL_SetWindowSize(win, width + BUTTON_WIN_W, height);
}

void save_image(char *scr_name)
{
   IMG_Init(IMG_INIT_PNG);
   SDL_Surface *scr = SDL_GetWindowSurface(win);
   IMG_SavePNG(scr, scr_name);
   SDL_FreeSurface(scr);
   IMG_Quit();
}


/* end of xwin_sdl.c */
